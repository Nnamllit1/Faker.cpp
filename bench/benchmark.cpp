#include <faker/faker.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr std::uint64_t kSeed = 123456789;

struct Options {
    int rows = 1000000;
    int runs = 1;
    std::string json_path = "performance.json";
    std::string markdown_path = "performance.md";
    std::string previous_path;
    std::string version = "local";
    std::string commit = "unknown";
    std::string os = "local";
};

struct PreviousMetrics {
    bool available = false;
    double seconds = 0.0;
    double rows_per_second = 0.0;
    double calls_per_second = 0.0;
};

struct SectionResult {
    std::string name;
    int rows = 0;
    int runs = 0;
    int calls_per_row = 0;
    long long total_calls = 0;
    double seconds = 0.0;
    double rows_per_second = 0.0;
    double calls_per_second = 0.0;
    double best_seconds = 0.0;
    double worst_seconds = 0.0;
    double average_seconds = 0.0;
    double median_seconds = 0.0;
    std::uint64_t checksum = 0;
    std::vector<double> run_seconds;
};

struct Result {
    int rows = 0;
    int runs = 0;
    int calls_per_row = 0;
    long long total_calls = 0;
    double seconds = 0.0;
    double rows_per_second = 0.0;
    double calls_per_second = 0.0;
    double best_seconds = 0.0;
    double worst_seconds = 0.0;
    double average_seconds = 0.0;
    double median_seconds = 0.0;
    std::uint64_t checksum = 0;
    std::vector<SectionResult> sections;
};

using SectionRunner = std::uint64_t (*)(faker::Faker&);

struct SectionSpec {
    const char* name;
    SectionRunner runner;
};

void usage() {
    std::cout << "faker_benchmark [options]\n"
              << "  --rows N              Number of rows to generate per section, default 1000000\n"
              << "  --runs N              Number of repeated benchmark runs, default 1\n"
              << "  --output-json PATH    JSON report path, default performance.json\n"
              << "  --output-md PATH      Markdown report path, default performance.md\n"
              << "  --previous PATH       Previous JSON report to compare against\n"
              << "  --version VALUE       Version or tag label\n"
              << "  --commit VALUE        Commit SHA label\n"
              << "  --os VALUE            OS label\n";
}

Options parse_args(int argc, char** argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        const auto need_value = [&](const char* name) -> std::string {
            if (i + 1 >= argc) {
                throw std::invalid_argument(std::string(name) + " requires a value");
            }
            return argv[++i];
        };

        if (arg == "--rows") {
            options.rows = std::stoi(need_value("--rows"));
        } else if (arg == "--runs") {
            options.runs = std::stoi(need_value("--runs"));
        } else if (arg == "--output-json") {
            options.json_path = need_value("--output-json");
        } else if (arg == "--output-md") {
            options.markdown_path = need_value("--output-md");
        } else if (arg == "--previous") {
            options.previous_path = need_value("--previous");
        } else if (arg == "--version") {
            options.version = need_value("--version");
        } else if (arg == "--commit") {
            options.commit = need_value("--commit");
        } else if (arg == "--os") {
            options.os = need_value("--os");
        } else if (arg == "--help" || arg == "-h") {
            usage();
            std::exit(0);
        } else {
            throw std::invalid_argument("unknown argument: " + arg);
        }
    }

    if (options.rows <= 0) {
        throw std::invalid_argument("--rows must be positive");
    }
    if (options.runs <= 0) {
        throw std::invalid_argument("--runs must be positive");
    }
    return options;
}

std::string compiler_name() {
#if defined(__clang__)
    return "Clang";
#elif defined(__GNUC__)
    return "GCC";
#elif defined(_MSC_VER)
    return "MSVC";
#else
    return "Unknown";
#endif
}

std::string compiler_version() {
    std::ostringstream out;
#if defined(__clang__)
    out << __clang_major__ << '.' << __clang_minor__ << '.' << __clang_patchlevel__;
#elif defined(__GNUC__)
    out << __GNUC__ << '.' << __GNUC_MINOR__ << '.' << __GNUC_PATCHLEVEL__;
#elif defined(_MSC_VER)
    out << _MSC_VER;
#else
    out << "unknown";
#endif
    return out.str();
}

std::string escape_json(const std::string& value) {
    std::string out;
    for (const auto ch : value) {
        switch (ch) {
        case '\\': out += "\\\\"; break;
        case '"': out += "\\\""; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default: out += ch; break;
        }
    }
    return out;
}

bool extract_number(const std::string& json, const std::string& key, double& value) {
    const auto key_pos = json.find('"' + key + '"');
    if (key_pos == std::string::npos) {
        return false;
    }
    const auto colon = json.find(':', key_pos);
    if (colon == std::string::npos) {
        return false;
    }
    const auto start = json.find_first_of("-0123456789", colon + 1);
    if (start == std::string::npos) {
        return false;
    }
    const auto end = json.find_first_not_of("0123456789.eE+-", start);
    value = std::stod(json.substr(start, end - start));
    return true;
}

PreviousMetrics read_previous(const std::string& path) {
    PreviousMetrics previous;
    if (path.empty()) {
        return previous;
    }

    std::ifstream file(path);
    if (!file) {
        return previous;
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    const auto json = buffer.str();

    previous.available =
        extract_number(json, "seconds", previous.seconds) &&
        extract_number(json, "rows_per_second", previous.rows_per_second) &&
        extract_number(json, "calls_per_second", previous.calls_per_second);
    return previous;
}

double percent_change(double current, double previous) {
    if (previous == 0.0) {
        return 0.0;
    }
    return ((current - previous) / previous) * 100.0;
}

std::string speed_text(double delta_percent) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2);
    if (delta_percent > 0.0) {
        out << delta_percent << "% faster";
    } else if (delta_percent < 0.0) {
        out << -delta_percent << "% slower";
    } else {
        out << "no change";
    }
    return out.str();
}

std::uint64_t string_size(const std::string& value) {
    return static_cast<std::uint64_t>(value.size());
}

std::uint64_t scaled_coordinate(double value, double offset) {
    return static_cast<std::uint64_t>((value + offset) * 1000.0);
}

const std::vector<SectionSpec>& sections() {
    static const std::vector<SectionSpec> specs = {
        {"boolean", [](faker::Faker& fake) { return static_cast<std::uint64_t>(fake.boolean() ? 1 : 0); }},
        {"number_int", [](faker::Faker& fake) { return static_cast<std::uint64_t>(fake.number_int(1, 1000)); }},
        {"number_real", [](faker::Faker& fake) { return static_cast<std::uint64_t>(fake.number_real(1.0, 1000.0)); }},
        {"uuid_v4", [](faker::Faker& fake) { return string_size(fake.uuid_v4()); }},
        {"first_name", [](faker::Faker& fake) { return string_size(fake.first_name()); }},
        {"last_name", [](faker::Faker& fake) { return string_size(fake.last_name()); }},
        {"full_name", [](faker::Faker& fake) { return string_size(fake.full_name()); }},
        {"name_prefix", [](faker::Faker& fake) { return string_size(fake.name_prefix()); }},
        {"name_suffix", [](faker::Faker& fake) { return string_size(fake.name_suffix()); }},
        {"middle_name", [](faker::Faker& fake) { return string_size(fake.middle_name()); }},
        {"username", [](faker::Faker& fake) { return string_size(fake.username()); }},
        {"email", [](faker::Faker& fake) { return string_size(fake.email()); }},
        {"domain_name", [](faker::Faker& fake) { return string_size(fake.domain_name()); }},
        {"url", [](faker::Faker& fake) { return string_size(fake.url()); }},
        {"ipv4", [](faker::Faker& fake) { return string_size(fake.ipv4()); }},
        {"ipv6", [](faker::Faker& fake) { return string_size(fake.ipv6()); }},
        {"mac_address", [](faker::Faker& fake) { return string_size(fake.mac_address()); }},
        {"password", [](faker::Faker& fake) { return string_size(fake.password()); }},
        {"user_agent", [](faker::Faker& fake) { return string_size(fake.user_agent()); }},
        {"phone_number", [](faker::Faker& fake) { return string_size(fake.phone_number()); }},
        {"building_number", [](faker::Faker& fake) { return string_size(fake.building_number()); }},
        {"street_name", [](faker::Faker& fake) { return string_size(fake.street_name()); }},
        {"street_address", [](faker::Faker& fake) { return string_size(fake.street_address()); }},
        {"secondary_address", [](faker::Faker& fake) { return string_size(fake.secondary_address()); }},
        {"city", [](faker::Faker& fake) { return string_size(fake.city()); }},
        {"country", [](faker::Faker& fake) { return string_size(fake.country()); }},
        {"state", [](faker::Faker& fake) { return string_size(fake.state()); }},
        {"zip_code", [](faker::Faker& fake) { return string_size(fake.zip_code()); }},
        {"postal_code", [](faker::Faker& fake) { return string_size(fake.postal_code()); }},
        {"latitude", [](faker::Faker& fake) { return scaled_coordinate(fake.latitude(), 90.0); }},
        {"longitude", [](faker::Faker& fake) { return scaled_coordinate(fake.longitude(), 180.0); }},
        {"company_name", [](faker::Faker& fake) { return string_size(fake.company_name()); }},
        {"job_title", [](faker::Faker& fake) { return string_size(fake.job_title()); }},
        {"word", [](faker::Faker& fake) { return string_size(fake.word()); }},
        {"sentence", [](faker::Faker& fake) { return string_size(fake.sentence()); }},
        {"paragraph", [](faker::Faker& fake) { return string_size(fake.paragraph()); }},
        {"past_date", [](faker::Faker& fake) { return string_size(fake.past_date()); }},
        {"future_date", [](faker::Faker& fake) { return string_size(fake.future_date()); }},
        {"recent_date", [](faker::Faker& fake) { return string_size(fake.recent_date()); }},
        {"credit_card_number", [](faker::Faker& fake) { return string_size(fake.credit_card_number()); }},
        {"currency_code", [](faker::Faker& fake) { return string_size(fake.currency_code()); }},
        {"iban", [](faker::Faker& fake) { return string_size(fake.iban()); }},
        {"product_name", [](faker::Faker& fake) { return string_size(fake.product_name()); }},
        {"product_category", [](faker::Faker& fake) { return string_size(fake.product_category()); }},
        {"price", [](faker::Faker& fake) { return string_size(fake.price()); }},
        {"hex_string", [](faker::Faker& fake) { return string_size(fake.hex_string()); }},
        {"md5", [](faker::Faker& fake) { return string_size(fake.md5()); }},
        {"sha1", [](faker::Faker& fake) { return string_size(fake.sha1()); }},
        {"sha256", [](faker::Faker& fake) { return string_size(fake.sha256()); }},
        {"file_name", [](faker::Faker& fake) { return string_size(fake.file_name()); }},
        {"file_path", [](faker::Faker& fake) { return string_size(fake.file_path()); }},
        {"mime_type", [](faker::Faker& fake) { return string_size(fake.mime_type()); }},
        {"semver", [](faker::Faker& fake) { return string_size(fake.semver()); }},
        {"hex_color", [](faker::Faker& fake) { return string_size(fake.hex_color()); }},
        {"rgb_color", [](faker::Faker& fake) { return string_size(fake.rgb_color()); }},
    };
    return specs;
}

SectionResult run_section_once(const SectionSpec& spec, int rows, int run_index) {
    faker::Faker fake(kSeed + static_cast<std::uint64_t>(run_index));
    volatile std::uint64_t sink = 0;

    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < rows; ++i) {
        sink += spec.runner(fake);
    }
    const auto end = std::chrono::steady_clock::now();

    SectionResult result;
    result.name = spec.name;
    result.rows = rows;
    result.runs = 1;
    result.calls_per_row = 1;
    result.total_calls = static_cast<long long>(rows) * result.calls_per_row;
    result.seconds = std::chrono::duration<double>(end - start).count();
    result.rows_per_second = static_cast<double>(rows) / result.seconds;
    result.calls_per_second = static_cast<double>(result.total_calls) / result.seconds;
    result.best_seconds = result.seconds;
    result.worst_seconds = result.seconds;
    result.average_seconds = result.seconds;
    result.median_seconds = result.seconds;
    result.checksum = sink;
    result.run_seconds.push_back(result.seconds);
    return result;
}

SectionResult run_section(const SectionSpec& spec, int rows, int runs) {
    std::vector<SectionResult> results;
    results.reserve(static_cast<std::size_t>(runs));
    for (int i = 0; i < runs; ++i) {
        results.push_back(run_section_once(spec, rows, i));
    }

    std::vector<double> sorted_seconds;
    sorted_seconds.reserve(results.size());
    double total_seconds = 0.0;
    std::uint64_t checksum = 0;
    for (const auto& result : results) {
        sorted_seconds.push_back(result.seconds);
        total_seconds += result.seconds;
        checksum += result.checksum;
    }
    std::sort(sorted_seconds.begin(), sorted_seconds.end());

    const auto middle = sorted_seconds.size() / 2;
    const auto median_seconds = sorted_seconds.size() % 2 == 0
        ? (sorted_seconds[middle - 1] + sorted_seconds[middle]) / 2.0
        : sorted_seconds[middle];

    SectionResult result;
    result.name = spec.name;
    result.rows = rows;
    result.runs = runs;
    result.calls_per_row = 1;
    result.total_calls = static_cast<long long>(rows) * result.calls_per_row;
    result.seconds = median_seconds;
    result.rows_per_second = static_cast<double>(rows) / result.seconds;
    result.calls_per_second = static_cast<double>(result.total_calls) / result.seconds;
    result.best_seconds = sorted_seconds.front();
    result.worst_seconds = sorted_seconds.back();
    result.average_seconds = total_seconds / static_cast<double>(results.size());
    result.median_seconds = median_seconds;
    result.checksum = checksum;
    result.run_seconds = sorted_seconds;
    return result;
}

Result run_benchmark(int rows, int runs) {
    Result result;
    result.rows = rows;
    result.runs = runs;

    for (const auto& spec : sections()) {
        auto section = run_section(spec, rows, runs);
        result.calls_per_row += section.calls_per_row;
        result.total_calls += section.total_calls;
        result.seconds += section.seconds;
        result.best_seconds += section.best_seconds;
        result.worst_seconds += section.worst_seconds;
        result.average_seconds += section.average_seconds;
        result.checksum += section.checksum;
        result.sections.push_back(section);
    }

    result.median_seconds = result.seconds;
    result.rows_per_second = static_cast<double>(rows) / result.seconds;
    result.calls_per_second = static_cast<double>(result.total_calls) / result.seconds;
    return result;
}

void write_run_seconds(std::ofstream& out, const std::vector<double>& values) {
    out << "[";
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            out << ", ";
        }
        out << values[i];
    }
    out << "]";
}

void write_section_json(std::ofstream& out, const SectionResult& section, const char* indent) {
    out << indent << "{\n";
    out << indent << "  \"name\": \"" << escape_json(section.name) << "\",\n";
    out << indent << "  \"rows\": " << section.rows << ",\n";
    out << indent << "  \"runs\": " << section.runs << ",\n";
    out << indent << "  \"calls_per_row\": " << section.calls_per_row << ",\n";
    out << indent << "  \"total_calls\": " << section.total_calls << ",\n";
    out << indent << "  \"total_calls_all_runs\": " << (section.total_calls * static_cast<long long>(section.runs)) << ",\n";
    out << indent << "  \"metric\": \"median\",\n";
    out << indent << "  \"seconds\": " << section.seconds << ",\n";
    out << indent << "  \"median_seconds\": " << section.median_seconds << ",\n";
    out << indent << "  \"best_seconds\": " << section.best_seconds << ",\n";
    out << indent << "  \"worst_seconds\": " << section.worst_seconds << ",\n";
    out << indent << "  \"average_seconds\": " << section.average_seconds << ",\n";
    out << indent << "  \"rows_per_second\": " << section.rows_per_second << ",\n";
    out << indent << "  \"calls_per_second\": " << section.calls_per_second << ",\n";
    out << indent << "  \"checksum\": " << section.checksum << ",\n";
    out << indent << "  \"run_seconds\": ";
    write_run_seconds(out, section.run_seconds);
    out << "\n" << indent << "}";
}

void write_json_report(const std::string& path, const Result& result, const Options& options, const PreviousMetrics& previous) {
    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("failed to open JSON report path: " + path);
    }

    const auto rows_delta = previous.available ? percent_change(result.rows_per_second, previous.rows_per_second) : 0.0;
    const auto calls_delta = previous.available ? percent_change(result.calls_per_second, previous.calls_per_second) : 0.0;
    const auto seconds_delta = previous.available ? percent_change(result.seconds, previous.seconds) : 0.0;

    out << std::fixed << std::setprecision(10);
    out << "{\n";
    out << "  \"version\": \"" << escape_json(options.version) << "\",\n";
    out << "  \"commit\": \"" << escape_json(options.commit) << "\",\n";
    out << "  \"os\": \"" << escape_json(options.os) << "\",\n";
    out << "  \"compiler\": \"" << escape_json(compiler_name()) << "\",\n";
    out << "  \"compiler_version\": \"" << escape_json(compiler_version()) << "\",\n";
    out << "  \"rows\": " << result.rows << ",\n";
    out << "  \"runs\": " << result.runs << ",\n";
    out << "  \"sections_per_row\": " << result.sections.size() << ",\n";
    out << "  \"calls_per_row\": " << result.calls_per_row << ",\n";
    out << "  \"total_calls\": " << result.total_calls << ",\n";
    out << "  \"total_calls_all_runs\": " << (result.total_calls * static_cast<long long>(result.runs)) << ",\n";
    out << "  \"metric\": \"median\",\n";
    out << "  \"seconds\": " << result.seconds << ",\n";
    out << "  \"median_seconds\": " << result.median_seconds << ",\n";
    out << "  \"best_seconds\": " << result.best_seconds << ",\n";
    out << "  \"worst_seconds\": " << result.worst_seconds << ",\n";
    out << "  \"average_seconds\": " << result.average_seconds << ",\n";
    out << "  \"rows_per_second\": " << result.rows_per_second << ",\n";
    out << "  \"calls_per_second\": " << result.calls_per_second << ",\n";
    out << "  \"checksum\": " << result.checksum << ",\n";
    out << "  \"previous_available\": " << (previous.available ? "true" : "false") << ",\n";
    out << "  \"seconds_delta_percent\": " << seconds_delta << ",\n";
    out << "  \"rows_per_second_delta_percent\": " << rows_delta << ",\n";
    out << "  \"calls_per_second_delta_percent\": " << calls_delta << ",\n";
    out << "  \"comparison\": \"" << escape_json(previous.available ? speed_text(rows_delta) : "no previous metric") << "\",\n";
    out << "  \"sections\": [\n";
    for (std::size_t i = 0; i < result.sections.size(); ++i) {
        if (i != 0) {
            out << ",\n";
        }
        write_section_json(out, result.sections[i], "    ");
    }
    out << "\n  ]\n";
    out << "}\n";
}

void write_markdown_report(
    const std::string& path,
    const Result& result,
    const Options& options,
    const PreviousMetrics& previous) {
    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("failed to open Markdown report path: " + path);
    }

    const auto rows_delta = previous.available ? percent_change(result.rows_per_second, previous.rows_per_second) : 0.0;

    out << std::fixed << std::setprecision(6);
    out << "# Faker.cpp Performance\n\n";
    out << "- Version: `" << options.version << "`\n";
    out << "- Commit: `" << options.commit << "`\n";
    out << "- OS: `" << options.os << "`\n";
    out << "- Compiler: `" << compiler_name() << ' ' << compiler_version() << "`\n";
    out << "- Rows per result: `" << result.rows << "`\n";
    out << "- Runs per result: `" << result.runs << "`\n";
    out << "- Comparison metric: `median`\n";
    out << "- Generated results per row: `" << result.calls_per_row << "`\n";
    out << "- Previous aggregate: `" << (previous.available ? speed_text(rows_delta) : "no previous metric") << "`\n\n";

    out << "## Aggregate\n\n";
    out << "| Median seconds | Best seconds | Rows/s | Calls/s | Total calls/run |\n";
    out << "| ---: | ---: | ---: | ---: | ---: |\n";
    out << "| " << result.seconds << " | "
        << result.best_seconds << " | "
        << std::setprecision(2) << result.rows_per_second << " | "
        << result.calls_per_second << " | "
        << result.total_calls << " |\n\n";

    out << "## Results\n\n";
    out << "| Result | Median seconds | Best seconds | Rows/s | Calls/s |\n";
    out << "| --- | ---: | ---: | ---: | ---: |\n";
    for (const auto& section : result.sections) {
        out << "| `" << section.name << "` | "
            << std::setprecision(6) << section.seconds << " | "
            << section.best_seconds << " | "
            << std::setprecision(2) << section.rows_per_second << " | "
            << section.calls_per_second << " |\n";
    }

    out << "\nEach named result is benchmarked independently. Adding a new generated value creates a new row instead of changing the workload of existing rows.\n";
}

void print_summary(const Result& result, const Options& options, const PreviousMetrics& previous) {
    const auto rows_delta = previous.available ? percent_change(result.rows_per_second, previous.rows_per_second) : 0.0;

    std::cout << std::fixed << std::setprecision(2)
              << "Faker.cpp benchmark " << options.os << " " << options.version << "\n"
              << "Rows/result: " << result.rows << ", runs/result: " << result.runs
              << ", results/row: " << result.calls_per_row << "\n"
              << "Aggregate median: " << std::setprecision(6) << result.seconds << " s, "
              << std::setprecision(2) << result.rows_per_second << " rows/s, "
              << result.calls_per_second << " calls/s\n"
              << "Previous aggregate: " << (previous.available ? speed_text(rows_delta) : "no previous metric") << "\n"
              << "Wrote " << options.json_path << " and " << options.markdown_path << "\n";
}

} // namespace

int main(int argc, char** argv) {
    try {
        const auto options = parse_args(argc, argv);
        const auto previous = read_previous(options.previous_path);
        const auto result = run_benchmark(options.rows, options.runs);

        write_json_report(options.json_path, result, options, previous);
        write_markdown_report(options.markdown_path, result, options, previous);
        print_summary(result, options, previous);
    } catch (const std::exception& error) {
        std::cerr << "faker_benchmark: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
