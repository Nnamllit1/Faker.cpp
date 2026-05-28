#!/usr/bin/env python3
import argparse
import json
from pathlib import Path


def percent_change(current, previous):
    if previous == 0:
        return 0.0
    return ((current - previous) / previous) * 100.0


def speed_text(delta):
    if delta > 0:
        return f"{delta:.2f}% faster"
    if delta < 0:
        return f"{abs(delta):.2f}% slower"
    return "no change"


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
        current["previous_available"] = previous is not None
        if previous:
            current["seconds_delta_percent"] = percent_change(current["seconds"], previous["seconds"])
            current["rows_per_second_delta_percent"] = percent_change(
                current["rows_per_second"], previous["rows_per_second"]
            )
            current["calls_per_second_delta_percent"] = percent_change(
                current["calls_per_second"], previous["calls_per_second"]
            )
            current["comparison"] = speed_text(current["rows_per_second_delta_percent"])
        else:
            current["seconds_delta_percent"] = 0.0
            current["rows_per_second_delta_percent"] = 0.0
            current["calls_per_second_delta_percent"] = 0.0
            current["comparison"] = "no previous metric"

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


def write_markdown(path, report):
    lines = [
        "# Faker.cpp Performance",
        "",
        f"- Version: `{report['version']}`",
        f"- Commit: `{report['commit']}`",
        f"- Rows per system: `{report['rows']}`",
        f"- Runs per system: `{report['runs']}`",
        f"- Comparison metric: `{report['metric']}`",
        f"- Calls per row: `{report['calls_per_row']}`",
        f"- Total calls per run: `{report['total_calls_per_system']}`",
        f"- Total calls across runs per system: `{report['total_calls_all_runs_per_system']}`",
        "",
        "| OS | Median seconds | Best seconds | Rows/s | Calls/s | Previous |",
        "| --- | ---: | ---: | ---: | ---: | --- |",
    ]

    for system in report["systems"]:
        lines.append(
            f"| {system['os']} | "
            f"{system['seconds']:.6f} | "
            f"{system.get('best_seconds', system['seconds']):.6f} | "
            f"{system['rows_per_second']:.2f} | "
            f"{system['calls_per_second']:.2f} | "
            f"{system['comparison']} |"
        )

    lines.extend(
        [
            "",
            "The benchmark generates every flat public Faker API once per row.",
            "Runner and VM numbers are best used for release-to-release comparison, not absolute hardware claims.",
            "",
        ]
    )

    Path(path).write_text("\n".join(lines), encoding="utf-8")


def write_release_body(path, report):
    lines = [
        f"Manual Faker.cpp release built from `{report['commit']}`.",
        "",
        "## Performance",
        "",
        f"Benchmark workload: `{report['runs']}` runs per OS, `{report['rows']}` rows per run, "
        f"`{report['calls_per_row']}` generated values per row. The table uses `{report['metric']}` results.",
        "",
        "| OS | Median seconds | Best seconds | Rows/s | Calls/s | Previous |",
        "| --- | ---: | ---: | ---: | ---: | --- |",
    ]

    for system in report["systems"]:
        lines.append(
            f"| {system['os']} | "
            f"{system['seconds']:.6f} | "
            f"{system.get('best_seconds', system['seconds']):.6f} | "
            f"{system['rows_per_second']:.2f} | "
            f"{system['calls_per_second']:.2f} | "
            f"{system['comparison']} |"
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
