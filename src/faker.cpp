#include "faker/faker.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string_view>

namespace faker {
namespace {

using table = std::initializer_list<std::string_view>;

constexpr std::array<std::string_view, 40> kFirstNames = {
    "Alex", "Avery", "Bailey", "Blair", "Cameron", "Casey", "Charlie", "Dakota",
    "Drew", "Elliot", "Emerson", "Finley", "Harper", "Hayden", "Jamie", "Jordan",
    "Kai", "Kendall", "Logan", "Morgan", "Parker", "Quinn", "Reese", "Riley",
    "River", "Rowan", "Sage", "Sam", "Sawyer", "Skyler", "Taylor", "Terry",
    "Toby", "Robin", "Noah", "Maya", "Iris", "Milo", "Nora", "Theo",
};

constexpr std::array<std::string_view, 40> kLastNames = {
    "Anderson", "Baker", "Bennett", "Brooks", "Carter", "Clark", "Coleman", "Cooper",
    "Davis", "Diaz", "Edwards", "Evans", "Fisher", "Foster", "Garcia", "Gray",
    "Green", "Hall", "Harris", "Hayes", "Hill", "Howard", "Hughes", "Jackson",
    "James", "Johnson", "Kelly", "King", "Lewis", "Martin", "Miller", "Mitchell",
    "Moore", "Nelson", "Parker", "Perez", "Reed", "Roberts", "Smith", "Turner",
};

constexpr std::array<std::string_view, 28> kWords = {
    "alpha", "brisk", "clear", "delta", "ember", "field", "globe", "harbor",
    "index", "jolt", "kernel", "lumen", "matrix", "north", "orbit", "pixel",
    "quiet", "river", "signal", "thread", "union", "vector", "window", "xenon",
    "yellow", "zenith", "module", "syntax",
};

constexpr std::array<std::string_view, 18> kCities = {
    "Berlin", "Hamburg", "Munich", "Cologne", "Frankfurt", "Stuttgart",
    "Dresden", "Leipzig", "Bremen", "Hannover", "Zurich", "Vienna",
    "Amsterdam", "Prague", "Warsaw", "Paris", "Madrid", "Milan",
};

constexpr std::array<std::string_view, 16> kCountries = {
    "Germany", "Austria", "Switzerland", "Netherlands", "France", "Spain",
    "Italy", "Poland", "Czechia", "Denmark", "Sweden", "Norway",
    "United Kingdom", "Ireland", "Canada", "United States",
};

constexpr std::array<std::string_view, 20> kStreetNames = {
    "Main", "Oak", "Maple", "Cedar", "Pine", "Market", "River", "Lake",
    "Hill", "Park", "King", "Queen", "Station", "Garden", "Forest", "Bridge",
    "Sunset", "Highland", "Liberty", "Central",
};

constexpr std::array<std::string_view, 18> kCompanyNouns = {
    "Systems", "Labs", "Works", "Networks", "Studios", "Foundry", "Dynamics",
    "Industries", "Solutions", "Analytics", "Forge", "Grid", "Cloud", "Logic",
    "Stack", "Circuit", "Shift", "Vector",
};

constexpr std::array<std::string_view, 18> kCompanyPrefixes = {
    "Nova", "Apex", "Blue", "Bright", "Core", "Future", "Green", "Hyper",
    "Iron", "Lunar", "Metro", "North", "Open", "Prime", "Quantum", "Rapid",
    "Silver", "True",
};

constexpr std::array<std::string_view, 16> kJobTitles = {
    "Software Engineer", "Product Manager", "Data Analyst", "Designer",
    "DevOps Engineer", "QA Engineer", "Technical Writer", "Support Lead",
    "Security Engineer", "Researcher", "Sales Manager", "Account Executive",
    "Operations Manager", "Frontend Engineer", "Backend Engineer", "Architect",
};

constexpr std::array<std::string_view, 10> kTlds = {
    "com", "net", "org", "dev", "io", "app", "cloud", "tech", "example", "test",
};

template <std::size_t N>
std::string pick(std::mt19937_64& rng, const std::array<std::string_view, N>& values) {
    std::uniform_int_distribution<std::size_t> dist(0, values.size() - 1);
    return std::string(values[dist(rng)]);
}

std::string lower_ascii(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::string remove_spaces(std::string value) {
    value.erase(std::remove_if(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    }), value.end());
    return value;
}

std::string two_digits(int value) {
    std::ostringstream out;
    out << std::setw(2) << std::setfill('0') << value;
    return out.str();
}

std::string format_date(Faker::clock::time_point time) {
    const auto raw = Faker::clock::to_time_t(time);
    std::tm tm_value{};
#if defined(_WIN32)
    gmtime_s(&tm_value, &raw);
#else
    gmtime_r(&raw, &tm_value);
#endif

    std::ostringstream out;
    out << (tm_value.tm_year + 1900) << '-'
        << two_digits(tm_value.tm_mon + 1) << '-'
        << two_digits(tm_value.tm_mday);
    return out.str();
}

std::uint64_t random_seed() {
    std::random_device device;
    const auto high = static_cast<std::uint64_t>(device());
    const auto low = static_cast<std::uint64_t>(device());
    return (high << 32U) ^ low;
}

} // namespace

Faker::Faker() : rng_(random_seed()) {}

Faker::Faker(std::uint64_t seed_value) : rng_(seed_value) {}

void Faker::seed(std::uint64_t value) {
    rng_.seed(value);
}

bool Faker::boolean(double probability) {
    if (probability < 0.0 || probability > 1.0) {
        throw std::invalid_argument("faker::Faker::boolean probability must be between 0 and 1");
    }
    return std::bernoulli_distribution(probability)(rng_);
}

int Faker::number_int(int min, int max) {
    if (min > max) {
        throw std::invalid_argument("faker::Faker::number_int min must be <= max");
    }
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng_);
}

double Faker::number_real(double min, double max) {
    if (min > max) {
        throw std::invalid_argument("faker::Faker::number_real min must be <= max");
    }
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng_);
}

std::string Faker::uuid_v4() {
    std::array<unsigned char, 16> bytes{};
    for (auto& byte : bytes) {
        byte = static_cast<unsigned char>(number_int(0, 255));
    }
    bytes[6] = static_cast<unsigned char>((bytes[6] & 0x0fU) | 0x40U);
    bytes[8] = static_cast<unsigned char>((bytes[8] & 0x3fU) | 0x80U);

    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        out << std::setw(2) << static_cast<int>(bytes[i]);
        if (i == 3 || i == 5 || i == 7 || i == 9) {
            out << '-';
        }
    }
    return out.str();
}

std::string Faker::first_name() {
    return pick(rng_, kFirstNames);
}

std::string Faker::last_name() {
    return pick(rng_, kLastNames);
}

std::string Faker::full_name() {
    return first_name() + " " + last_name();
}

std::string Faker::username() {
    return lower_ascii(first_name()) + "." + lower_ascii(last_name()) + std::to_string(number_int(10, 9999));
}

std::string Faker::email() {
    return username() + "@" + domain_name();
}

std::string Faker::domain_name() {
    return remove_spaces(lower_ascii(pick(rng_, kCompanyPrefixes))) + "-" +
        remove_spaces(lower_ascii(pick(rng_, kCompanyNouns))) + "." + pick(rng_, kTlds);
}

std::string Faker::url() {
    return "https://www." + domain_name();
}

std::string Faker::ipv4() {
    return std::to_string(number_int(1, 223)) + "." +
        std::to_string(number_int(0, 255)) + "." +
        std::to_string(number_int(0, 255)) + "." +
        std::to_string(number_int(1, 254));
}

std::string Faker::phone_number() {
    std::ostringstream out;
    out << "+1-" << number_int(200, 999) << '-' << number_int(200, 999) << '-'
        << std::setw(4) << std::setfill('0') << number_int(0, 9999);
    return out.str();
}

std::string Faker::street_address() {
    return std::to_string(number_int(1, 9999)) + " " + pick(rng_, kStreetNames) + " Street";
}

std::string Faker::city() {
    return pick(rng_, kCities);
}

std::string Faker::country() {
    return pick(rng_, kCountries);
}

std::string Faker::company_name() {
    return pick(rng_, kCompanyPrefixes) + " " + pick(rng_, kCompanyNouns);
}

std::string Faker::job_title() {
    return pick(rng_, kJobTitles);
}

std::string Faker::word() {
    return pick(rng_, kWords);
}

std::string Faker::sentence(int words) {
    if (words <= 0) {
        throw std::invalid_argument("faker::Faker::sentence words must be positive");
    }

    std::string value;
    for (int i = 0; i < words; ++i) {
        if (i != 0) {
            value += ' ';
        }
        value += word();
    }
    value[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(value[0])));
    value += '.';
    return value;
}

std::string Faker::paragraph(int sentences) {
    if (sentences <= 0) {
        throw std::invalid_argument("faker::Faker::paragraph sentences must be positive");
    }

    std::string value;
    for (int i = 0; i < sentences; ++i) {
        if (i != 0) {
            value += ' ';
        }
        value += sentence(number_int(6, 14));
    }
    return value;
}

std::string Faker::date_between(clock::time_point from, clock::time_point to) {
    if (from > to) {
        throw std::invalid_argument("faker::Faker::date_between from must be <= to");
    }

    const auto from_seconds = std::chrono::duration_cast<std::chrono::seconds>(from.time_since_epoch()).count();
    const auto to_seconds = std::chrono::duration_cast<std::chrono::seconds>(to.time_since_epoch()).count();
    std::uniform_int_distribution<long long> dist(from_seconds, to_seconds);
    return format_date(clock::time_point(std::chrono::seconds(dist(rng_))));
}

std::string Faker::past_date(int days_back) {
    if (days_back < 0) {
        throw std::invalid_argument("faker::Faker::past_date days_back must be non-negative");
    }
    const auto now = clock::now();
    return date_between(now - std::chrono::hours(24 * days_back), now);
}

std::string Faker::future_date(int days_forward) {
    if (days_forward < 0) {
        throw std::invalid_argument("faker::Faker::future_date days_forward must be non-negative");
    }
    const auto now = clock::now();
    return date_between(now, now + std::chrono::hours(24 * days_forward));
}

std::string Faker::recent_date(int days_back) {
    return past_date(days_back);
}

Faker& default_faker() {
    thread_local Faker instance;
    return instance;
}

void seed(std::uint64_t value) { default_faker().seed(value); }
bool boolean(double probability) { return default_faker().boolean(probability); }
int number_int(int min, int max) { return default_faker().number_int(min, max); }
double number_real(double min, double max) { return default_faker().number_real(min, max); }
std::string uuid_v4() { return default_faker().uuid_v4(); }
std::string first_name() { return default_faker().first_name(); }
std::string last_name() { return default_faker().last_name(); }
std::string full_name() { return default_faker().full_name(); }
std::string username() { return default_faker().username(); }
std::string email() { return default_faker().email(); }
std::string domain_name() { return default_faker().domain_name(); }
std::string url() { return default_faker().url(); }
std::string ipv4() { return default_faker().ipv4(); }
std::string phone_number() { return default_faker().phone_number(); }
std::string street_address() { return default_faker().street_address(); }
std::string city() { return default_faker().city(); }
std::string country() { return default_faker().country(); }
std::string company_name() { return default_faker().company_name(); }
std::string job_title() { return default_faker().job_title(); }
std::string word() { return default_faker().word(); }
std::string sentence(int words) { return default_faker().sentence(words); }
std::string paragraph(int sentences) { return default_faker().paragraph(sentences); }
std::string date_between(Faker::clock::time_point from, Faker::clock::time_point to) { return default_faker().date_between(from, to); }
std::string past_date(int days_back) { return default_faker().past_date(days_back); }
std::string future_date(int days_forward) { return default_faker().future_date(days_forward); }
std::string recent_date(int days_back) { return default_faker().recent_date(days_back); }

} // namespace faker

