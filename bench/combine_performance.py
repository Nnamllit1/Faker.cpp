#!/usr/bin/env python3
import argparse
import json
from pathlib import Path


VARIANCE_THRESHOLD_PERCENT = 5.0


def percent_change(current, previous):
    if previous == 0:
        return 0.0
    return ((current - previous) / previous) * 100.0


def direction_text(delta):
    if delta > 0:
        return f"{delta:.2f}% faster"
    if delta < 0:
        return f"{abs(delta):.2f}% slower"
    return "no change"


def classify_speed(delta):
    if delta >= VARIANCE_THRESHOLD_PERCENT:
        return "improved", direction_text(delta)
    if delta <= -VARIANCE_THRESHOLD_PERCENT:
        return "slower", f"possible regression ({direction_text(delta)})"
    if delta == 0:
        return "within_variance", "within runtime variance"
    return "within_variance", f"within runtime variance ({direction_text(delta)})"


def copy_previous_metrics(current, previous):
    current["previous_seconds"] = previous.get("seconds")
    current["previous_rows_per_second"] = previous.get("rows_per_second")
    current["previous_calls_per_second"] = previous.get("calls_per_second")
    current["previous_calls_per_row"] = previous.get("calls_per_row")


def apply_comparison(current, previous, missing_text, missing_status):
    current["previous_available"] = previous is not None
    current["previous_comparable"] = False

    if not previous:
        current["seconds_delta_percent"] = 0.0
        current["rows_per_second_delta_percent"] = 0.0
        current["calls_per_second_delta_percent"] = 0.0
        current["comparison"] = missing_text
        current["comparison_status"] = missing_status
        return

    copy_previous_metrics(current, previous)

    if current.get("calls_per_row") != previous.get("calls_per_row"):
        current["seconds_delta_percent"] = 0.0
        current["rows_per_second_delta_percent"] = 0.0
        current["calls_per_second_delta_percent"] = 0.0
        current["comparison"] = "workload changed"
        current["comparison_status"] = "workload_changed"
        return

    current["previous_comparable"] = True
    current["seconds_delta_percent"] = percent_change(current["seconds"], previous["seconds"])
    current["rows_per_second_delta_percent"] = percent_change(
        current["rows_per_second"], previous["rows_per_second"]
    )
    current["calls_per_second_delta_percent"] = percent_change(
        current["calls_per_second"], previous["calls_per_second"]
    )
    status, comparison = classify_speed(current["rows_per_second_delta_percent"])
    current["comparison"] = comparison
    current["comparison_status"] = status


def compare_sections(current, previous):
    previous_sections = {}
    if previous:
        previous_sections = {
            section.get("name"): section
            for section in previous.get("sections", [])
            if section.get("name")
        }

    for section in current.get("sections", []):
        apply_comparison(section, previous_sections.get(section.get("name")), "new result", "new_result")


def same_result_set(current, previous):
    current_names = sorted(section.get("name") for section in current.get("sections", []) if section.get("name"))
    previous_names = sorted(section.get("name") for section in previous.get("sections", []) if section.get("name"))
    return not current_names or not previous_names or current_names == previous_names


def load_previous(path):
    if not path:
        return {}
    previous_path = Path(path)
    if not previous_path.exists():
        return {}
    with previous_path.open("r", encoding="utf-8") as file:
        data = json.load(file)
    return {item.get("os"): item for item in data.get("systems", [])}


def combine_metrics(args):
    previous_by_os = load_previous(args.previous)
    systems = []

    for metric_path in sorted(Path(path) for path in args.metrics):
        with metric_path.open("r", encoding="utf-8") as file:
            current = json.load(file)

        previous = previous_by_os.get(current.get("os"))
        apply_comparison(current, previous, "no previous metric", "no_previous_metric")
        if previous and not same_result_set(current, previous):
            current["previous_comparable"] = False
            current["seconds_delta_percent"] = 0.0
            current["rows_per_second_delta_percent"] = 0.0
            current["calls_per_second_delta_percent"] = 0.0
            current["comparison"] = "workload changed"
            current["comparison_status"] = "workload_changed"
        compare_sections(current, previous)

        systems.append(current)

    if not systems:
        raise RuntimeError("no performance metrics were provided")

    return {
        "version": args.version,
        "commit": args.commit,
        "rows": systems[0]["rows"],
        "runs": systems[0].get("runs", 1),
        "metric": systems[0].get("metric", "single"),
        "calls_per_row": systems[0]["calls_per_row"],
        "sections_per_row": systems[0].get("sections_per_row", len(systems[0].get("sections", []))),
        "total_calls_per_system": systems[0]["total_calls"],
        "total_calls_all_runs_per_system": systems[0].get(
            "total_calls_all_runs", systems[0]["total_calls"] * systems[0].get("runs", 1)
        ),
        "systems": systems,
    }


def write_json(path, report):
    with Path(path).open("w", encoding="utf-8") as file:
        json.dump(report, file, indent=2)
        file.write("\n")


def result_table(systems):
    lines = [
        "| OS | Median seconds | Best seconds | Rows/s | Calls/s | Previous |",
        "| --- | ---: | ---: | ---: | ---: | --- |",
    ]
    for system in systems:
        lines.append(
            f"| {system['os']} | "
            f"{system['seconds']:.6f} | "
            f"{system.get('best_seconds', system['seconds']):.6f} | "
            f"{system['rows_per_second']:.2f} | "
            f"{system['calls_per_second']:.2f} | "
            f"{system['comparison']} |"
        )
    return lines


def section_table(system):
    lines = [
        "| Result | Median seconds | Best seconds | Rows/s | Calls/s | Previous |",
        "| --- | ---: | ---: | ---: | ---: | --- |",
    ]
    for section in system.get("sections", []):
        lines.append(
            f"| `{section['name']}` | "
            f"{section['seconds']:.6f} | "
            f"{section.get('best_seconds', section['seconds']):.6f} | "
            f"{section['rows_per_second']:.2f} | "
            f"{section['calls_per_second']:.2f} | "
            f"{section['comparison']} |"
        )
    return lines


def details_block(summary, lines):
    return [
        "<details>",
        f"<summary>{summary}</summary>",
        "",
        *lines,
        "",
        "</details>",
    ]


def status_counts(system):
    counts = {}
    for section in system.get("sections", []):
        status = section.get("comparison_status", "unknown")
        counts[status] = counts.get(status, 0) + 1
    return counts


def improved_sections(system, limit=5):
    sections = [
        section
        for section in system.get("sections", [])
        if section.get("comparison_status") == "improved"
    ]
    sections.sort(key=lambda section: section.get("rows_per_second_delta_percent", 0.0), reverse=True)
    return sections[:limit]


def new_sections(system, limit=8):
    sections = [
        section
        for section in system.get("sections", [])
        if section.get("comparison_status") == "new_result"
    ]
    return sections[:limit]


def workload_change_notes(report):
    changed_systems = [
        system for system in report["systems"] if system.get("comparison_status") == "workload_changed"
    ]
    if not changed_systems:
        return []

    counts = {
        (system.get("previous_calls_per_row"), system.get("calls_per_row"))
        for system in changed_systems
    }
    if len(counts) == 1:
        previous_count, current_count = next(iter(counts))
        if previous_count and current_count and previous_count != current_count:
            direction = "grew" if current_count > previous_count else "changed"
            return [
                f"The benchmark workload {direction} from {previous_count} to {current_count} "
                "generated values per row, so aggregate comparison is marked as `workload changed`."
            ]

    return [
        f"The benchmark workload changed on {system['os']}, so aggregate comparison is marked as `workload changed`."
        for system in changed_systems
    ]


def aggregate_time_note(report):
    changed_systems = [
        system
        for system in report["systems"]
        if system.get("comparison_status") == "workload_changed"
        and system.get("previous_seconds") is not None
    ]
    if not changed_systems:
        return None

    parts = []
    all_improved = True
    for system in changed_systems:
        previous_seconds = system["previous_seconds"]
        current_seconds = system["seconds"]
        all_improved = all_improved and current_seconds < previous_seconds
        parts.append(f"{system['os']} {previous_seconds:.3f}s -> {current_seconds:.3f}s")

    grew = all(
        system.get("previous_calls_per_row")
        and system.get("calls_per_row")
        and system.get("calls_per_row") > system.get("previous_calls_per_row")
        for system in changed_systems
    )
    workload_text = "larger workload" if grew else "changed workload"
    prefix = f"Despite the {workload_text}, total median time improved" if all_improved else "Total median time changed"
    return f"{prefix}: {', '.join(parts)}."


def top_improvement_note(report):
    entries = []
    for system in report["systems"]:
        for section in improved_sections(system, 5):
            entries.append((section.get("rows_per_second_delta_percent", 0.0), system["os"], section))
    entries.sort(key=lambda item: item[0], reverse=True)
    if not entries:
        return None

    formatted = [
        f"`{section['name']}` on {os_name} ({section['comparison']})"
        for _, os_name, section in entries[:5]
    ]
    return f"The biggest wins are {', '.join(formatted)}."


def comparison_summary_lines(report):
    lines = []
    for system in report["systems"]:
        counts = status_counts(system)
        lines.append(
            f"- `{system['os']}`: {counts.get('improved', 0)} improved, "
            f"{counts.get('slower', 0)} possible regressions, "
            f"{counts.get('within_variance', 0)} within runtime variance, "
            f"{counts.get('new_result', 0)} new results."
        )
    return lines


def write_markdown(path, report):
    lines = [
        "# Faker.cpp Performance",
        "",
        f"- Version: `{report['version']}`",
        f"- Commit: `{report['commit']}`",
        f"- Rows per system: `{report['rows']}`",
        f"- Runs per system: `{report['runs']}`",
        f"- Comparison metric: `{report['metric']}`",
        f"- Generated results per row: `{report['calls_per_row']}`",
        f"- Total calls per run: `{report['total_calls_per_system']}`",
        f"- Total calls across runs per system: `{report['total_calls_all_runs_per_system']}`",
        "",
        f"Small changes under {VARIANCE_THRESHOLD_PERCENT:.0f}% are treated as normal runner/runtime variation.",
        "Aggregate rows are marked `workload changed` when the generated-result count differs from the previous release. Use the per-result tables for regression checks.",
        "",
        "## Aggregate",
        "",
        *result_table(report["systems"]),
        "",
        "## Per-Generator Results",
    ]

    for system in report["systems"]:
        lines.extend(
            [
                "",
                *details_block(f"{system['os']} full result table", section_table(system)),
            ]
        )

    lines.extend(
        [
            "",
            "The benchmark generates each flat public Faker API in its own named result section.",
            "Runner and VM numbers are best used for release-to-release comparison, not absolute hardware claims.",
            "",
        ]
    )

    Path(path).write_text("\n".join(lines), encoding="utf-8")


def write_release_body(path, report):
    workload_notes = workload_change_notes(report)
    time_note = aggregate_time_note(report)
    improvement_note = top_improvement_note(report)

    lines = [
        f"Manual Faker.cpp release built from `{report['commit']}`. The planned stable release target remains `v0.1.0`; this project is still unlicensed.",
        "",
        "## Release Highlights",
        "",
        "> **What changed:** Performance reporting now separates aggregate workload context from per-generator comparisons.",
        ">",
        f"> **What improved:** Comparable benchmark changes under {VARIANCE_THRESHOLD_PERCENT:.0f}% are shown as runtime variance instead of regressions.",
        ">",
        "> **What is new:** Full Linux and Windows per-generator tables are available in expandable sections below.",
        ">",
        "> **Full metrics:** Download `performance.json` for machine-readable data or `performance.md` for the complete Markdown report.",
        "",
        "## Performance Summary",
        "",
        f"Benchmark workload: `{report['runs']}` runs per OS, `{report['rows']}` rows per run, "
        f"`{report['calls_per_row']}` generated values per row. The tables use `{report['metric']}` results.",
        "",
        f"Small per-generator movements below {VARIANCE_THRESHOLD_PERCENT:.0f}% are treated as runtime variance.",
        "",
    ]

    for note in workload_notes:
        lines.extend([note, ""])
    if time_note:
        lines.extend([time_note, ""])
    if improvement_note:
        lines.extend([improvement_note, ""])

    lines.extend(
        [
            "Aggregate rows are only directly comparable when the generated-result count is unchanged. Per-result rows show matching generators independently.",
            "",
            *result_table(report["systems"]),
            "",
            "## New Benchmark Results",
            "",
            *comparison_summary_lines(report),
        ]
    )

    for system in report["systems"]:
        new_results = new_sections(system)
        if not new_results:
            continue
        suffix = ""
        new_count = status_counts(system).get("new_result", 0)
        if new_count > len(new_results):
            suffix = f" and {new_count - len(new_results)} more"
        lines.append(
            f"- `{system['os']}` new rows: "
            + ", ".join(f"`{section['name']}`" for section in new_results)
            + suffix
            + "."
        )

    lines.extend(["", "## Full Per-Generator Results"])

    for system in report["systems"]:
        lines.extend(
            [
                "",
                *details_block(f"{system['os']} full result table", section_table(system)),
            ]
        )

    lines.extend(
        [
            "",
            "Full machine-readable and Markdown reports are attached as `performance.json` and `performance.md`.",
            "",
        ]
    )

    Path(path).write_text("\n".join(lines), encoding="utf-8")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--version", required=True)
    parser.add_argument("--commit", required=True)
    parser.add_argument("--previous")
    parser.add_argument("--output-json", required=True)
    parser.add_argument("--output-md", required=True)
    parser.add_argument("--release-body", required=True)
    parser.add_argument("metrics", nargs="+")
    args = parser.parse_args()

    report = combine_metrics(args)
    write_json(args.output_json, report)
    write_markdown(args.output_md, report)
    write_release_body(args.release_body, report)


if __name__ == "__main__":
    main()
