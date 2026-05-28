#include <faker/faker.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr long long kCallsPerRow = 48;

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

void usage() {
    std::cout << "faker_benchmark [options]\n"
              << "  --rows N              Number of everything rows to generate, default 1000000\n"
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

void add_string_size(volatile std::uint64_t& sink, const std::string& value) {
    sink += value.size();
}

struct Result {
    int rows = 0;
    int runs = 0;
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

Result run_benchmark_once(int rows) {
    faker::Faker fake(123456789);
    volatile std::uint64_t sink = 0;

    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < rows; ++i) {
        sink += fake.boolean();
        sink += static_cast<std::uint64_t>(fake.number_int(1, 1000));
        sink += static_cast<std::uint64_t>(fake.number_real(1.0, 1000.0));
        add_string_size(sink, fake.uuid_v4());
        add_string_size(sink, fake.first_name());
        add_string_size(sink, fake.last_name());
        add_string_size(sink, fake.full_name());
        add_string_size(sink, fake.username());
        add_string_size(sink, fake.email());
        add_string_size(sink, fake.domain_name());
        add_string_size(sink, fake.url());
        add_string_size(sink, fake.ipv4());
        add_string_size(sink, fake.ipv6());
        add_string_size(sink, fake.mac_address());
        add_string_size(sink, fake.password());
        add_string_size(sink, fake.user_agent());
        add_string_size(sink, fake.phone_number());
        add_string_size(sink, fake.street_address());
        add_string_size(sink, fake.city());
        add_string_size(sink, fake.country());
        add_string_size(sink, fake.state());
        add_string_size(sink, fake.zip_code());
        sink += static_cast<std::uint64_t>((fake.latitude() + 90.0) * 1000.0);
        sink += static_cast<std::uint64_t>((fake.longitude() + 180.0) * 1000.0);
        add_string_size(sink, fake.company_name());
        add_string_size(sink, fake.job_title());
        add_string_size(sink, fake.word());
        add_string_size(sink, fake.sentence());
        add_string_size(sink, fake.paragraph());
        add_string_size(sink, fake.past_date());
        add_string_size(sink, fake.future_date());
        add_string_size(sink, fake.recent_date());
        add_string_size(sink, fake.credit_card_number());
        add_string_size(sink, fake.currency_code());
        add_string_size(sink, fake.iban());
        add_string_size(sink, fake.product_name());
        add_string_size(sink, fake.product_category());
        add_string_size(sink, fake.price());
        add_string_size(sink, fake.hex_string());
        add_string_size(sink, fake.md5());
        add_string_size(sink, fake.sha1());
        add_string_size(sink, fake.sha256());
        add_string_size(sink, fake.file_name());
        add_string_size(sink, fake.file_path());
        add_string_size(sink, fake.mime_type());
        add_string_size(sink, fake.semver());
        add_string_size(sink, fake.hex_color());
        add_string_size(sink, fake.rgb_color());
    }
    const auto end = std::chrono::steady_clock::now();

    Result result;
    result.rows = rows;
    result.runs = 1;
    result.total_calls = static_cast<long long>(rows) * kCallsPerRow;
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

Result run_benchmark(int rows, int runs) {
    std::vector<Result> results;
    results.reserve(static_cast<std::size_t>(runs));
    for (int i = 0; i < runs; ++i) {
        results.push_back(run_benchmark_once(rows));
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

    Result result;
    result.rows = rows;
    result.runs = runs;
    result.total_calls = static_cast<long long>(rows) * kCallsPerRow;
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

void write_json(const Options& options, const Result& result, const PreviousMetrics& previous) {
    const double seconds_delta = previous.available ? percent_change(result.seconds, previous.seconds) : 0.0;
    const double rows_delta = previous.available ? percent_change(result.rows_per_second, previous.rows_per_second) : 0.0;
    const double calls_delta = previous.available ? percent_change(result.calls_per_second, previous.calls_per_second) : 0.0;

    std::ofstream out(options.json_path);
    out << std::fixed << std::setprecision(6);
    out << "{\n";
    out << "  \"version\": \"" << escape_json(options.version) << "\",\n";
    out << "  \"commit\": \"" << escape_json(options.commit) << "\",\n";
    out << "  \"os\": \"" << escape_json(options.os) << "\",\n";
    out << "  \"compiler\": \"" << compiler_name() << "\",\n";
    out << "  \"compiler_version\": \"" << compiler_version() << "\",\n";
    out << "  \"rows\": " << result.rows << ",\n";
    out << "  \"runs\": " << result.runs << ",\n";
    out << "  \"calls_per_row\": " << kCallsPerRow << ",\n";
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
    out << "  \"comparison\": \"" << (previous.available ? escape_json(speed_text(rows_delta)) : "no previous metric") << "\",\n";
    out << "  \"run_seconds\": [";
    for (std::size_t i = 0; i < result.run_seconds.size(); ++i) {
        if (i != 0) {
            out << ", ";
        }
        out << result.run_seconds[i];
    }
    out << "]\n";
    out << "}\n";
}

void write_markdown(const Options& options, const Result& result, const PreviousMetrics& previous) {
    const double rows_delta = previous.available ? percent_change(result.rows_per_second, previous.rows_per_second) : 0.0;
    const double calls_delta = previous.available ? percent_change(result.calls_per_second, previous.calls_per_second) : 0.0;

    std::ofstream out(options.markdown_path);
    out << "# Faker.cpp Performance\n\n";
    out << "- Version: `" << options.version << "`\n";
    out << "- Commit: `" << options.commit << "`\n";
    out << "- OS: `" << options.os << "`\n";
    out << "- Compiler: `" << compiler_name() << ' ' << compiler_version() << "`\n";
    out << "- Rows: `" << result.rows << "`\n";
    out << "- Runs: `" << result.runs << "`\n";
    out << "- Calls per row: `" << kCallsPerRow << "`\n";
    out << "- Total calls per run: `" << result.total_calls << "`\n";
    out << "- Total calls across runs: `" << (result.total_calls * static_cast<long long>(result.runs)) << "`\n";
    out << "- Metric: `median`\n";
    out << "- Median seconds: `" << std::fixed << std::setprecision(6) << result.median_seconds << "`\n";
    out << "- Best seconds: `" << result.best_seconds << "`\n";
    out << "- Worst seconds: `" << result.worst_seconds << "`\n";
    out << "- Average seconds: `" << result.average_seconds << "`\n";
    out << "- Rows/second: `" << result.rows_per_second << "`\n";
    out << "- Calls/second: `" << result.calls_per_second << "`\n";
    out << "- Checksum: `" << result.checksum << "`\n";
    if (previous.available) {
        out << "- Previous comparison: `" << speed_text(rows_delta) << "` by rows/second\n";
        out << "- Calls/second delta: `" << speed_text(calls_delta) << "`\n";
    } else {
        out << "- Previous comparison: `no previous metric`\n";
    }
}

} // namespace

int main(int argc, char** argv) {
    try {
        const auto options = parse_args(argc, argv);
        const auto previous = read_previous(options.previous_path);
        const auto result = run_benchmark(options.rows, options.runs);

        write_json(options, result, previous);
        write_markdown(options, result, previous);

        std::cout << "rows=" << result.rows << '\n';
        std::cout << "runs=" << result.runs << '\n';
        std::cout << "calls_per_row=" << kCallsPerRow << '\n';
        std::cout << "total_calls=" << result.total_calls << '\n';
        std::cout << "total_calls_all_runs=" << (result.total_calls * static_cast<long long>(result.runs)) << '\n';
        std::cout << "seconds=" << result.seconds << '\n';
        std::cout << "median_seconds=" << result.median_seconds << '\n';
        std::cout << "best_seconds=" << result.best_seconds << '\n';
        std::cout << "worst_seconds=" << result.worst_seconds << '\n';
        std::cout << "average_seconds=" << result.average_seconds << '\n';
        std::cout << "rows_per_second=" << result.rows_per_second << '\n';
        std::cout << "calls_per_second=" << result.calls_per_second << '\n';
        std::cout << "checksum=" << result.checksum << '\n';
        std::cout << "json=" << options.json_path << '\n';
        std::cout << "markdown=" << options.markdown_path << '\n';
        std::cout << "comparison=";
        if (previous.available) {
            std::cout << speed_text(percent_change(result.rows_per_second, previous.rows_per_second));
        } else {
            std::cout << "no previous metric";
        }
        std::cout << '\n';
    } catch (const std::exception& error) {
        std::cerr << "faker_benchmark: " << error.what() << '\n';
        return 1;
    }
}
