#include "faker/faker.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <random>
#include <sstream>
#include <string_view>

namespace faker {
namespace {

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

constexpr std::array<std::string_view, 18> kStates = {
    "California", "Texas", "Florida", "New York", "Pennsylvania", "Illinois",
    "Ohio", "Georgia", "North Carolina", "Michigan", "Bavaria", "Saxony",
    "Hesse", "Brandenburg", "Ontario", "Quebec", "Zurich", "Vienna",
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

constexpr std::array<std::string_view, 6> kUserAgents = {
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 Chrome/125.0 Safari/537.36",
    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 Chrome/125.0 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:126.0) Gecko/20100101 Firefox/126.0",
    "Mozilla/5.0 (X11; Linux x86_64; rv:126.0) Gecko/20100101 Firefox/126.0",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 14_5) AppleWebKit/605.1.15 Safari/605.1.15",
    "FakerCpp/0.1 (+https://github.com/Nnamllit1/Faker.cpp)",
};

constexpr std::array<std::string_view, 12> kCurrencyCodes = {
    "USD", "EUR", "GBP", "CHF", "CAD", "AUD", "JPY", "SEK", "NOK", "DKK", "PLN", "CZK",
};

constexpr std::array<std::string_view, 10> kProductAdjectives = {
    "Compact", "Durable", "Smart", "Wireless", "Portable", "Premium", "Eco", "Rapid", "Modular", "Classic",
};

constexpr std::array<std::string_view, 12> kProductNouns = {
    "Keyboard", "Monitor", "Backpack", "Notebook", "Speaker", "Router",
    "Desk", "Chair", "Camera", "Adapter", "Sensor", "Controller",
};

constexpr std::array<std::string_view, 10> kProductCategories = {
    "Electronics", "Office", "Home", "Tools", "Outdoor", "Gaming", "Audio", "Storage", "Networking", "Accessories",
};

constexpr std::array<std::string_view, 12> kFileExtensions = {
    "txt", "json", "csv", "md", "cpp", "hpp", "png", "jpg", "pdf", "log", "xml", "yaml",
};

constexpr std::array<std::string_view, 10> kMimeTypes = {
    "text/plain", "text/markdown", "text/csv", "application/json", "application/xml",
    "application/pdf", "image/png", "image/jpeg", "application/zip", "application/octet-stream",
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

std::string slug(std::string value) {
    value = lower_ascii(value);
    std::replace(value.begin(), value.end(), ' ', '-');
    return value;
}

std::string zero_padded(int value, int width) {
    std::ostringstream out;
    out << std::setw(width) << std::setfill('0') << value;
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
        << zero_padded(tm_value.tm_mon + 1, 2) << '-'
        << zero_padded(tm_value.tm_mday, 2);
    return out.str();
}

std::uint64_t random_seed() {
    std::random_device device;
    const auto high = static_cast<std::uint64_t>(device());
    const auto low = static_cast<std::uint64_t>(device());
    return (high << 32U) ^ low;
}

int luhn_check_digit(const std::string& digits) {
    int sum = 0;
    bool double_digit = true;
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        int value = *it - '0';
        if (double_digit) {
            value *= 2;
            if (value > 9) {
                value -= 9;
            }
        }
        sum += value;
        double_digit = !double_digit;
    }
    return (10 - (sum % 10)) % 10;
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

std::string Faker::ipv6() {
    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (int i = 0; i < 8; ++i) {
        if (i != 0) {
            out << ':';
        }
        out << std::setw(4) << number_int(0, 65535);
    }
    return out.str();
}

std::string Faker::mac_address() {
    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (int i = 0; i < 6; ++i) {
        if (i != 0) {
            out << ':';
        }
        out << std::setw(2) << number_int(0, 255);
    }
    return out.str();
}

std::string Faker::password(int length) {
    if (length <= 0) {
        throw std::invalid_argument("faker::Faker::password length must be positive");
    }

    constexpr std::string_view chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    std::string value;
    value.reserve(static_cast<std::size_t>(length));
    for (int i = 0; i < length; ++i) {
        value += chars[static_cast<std::size_t>(number_int(0, static_cast<int>(chars.size() - 1)))];
    }
    return value;
}

std::string Faker::user_agent() {
    return pick(rng_, kUserAgents);
}

std::string Faker::phone_number() {
    std::ostringstream out;
    out << "+1-" << number_int(200, 999) << '-' << number_int(200, 999) << '-'
        << zero_padded(number_int(0, 9999), 4);
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

std::string Faker::state() {
    return pick(rng_, kStates);
}

std::string Faker::zip_code() {
    return zero_padded(number_int(0, 99999), 5);
}

double Faker::latitude() {
    return number_real(-90.0, 90.0);
}

double Faker::longitude() {
    return number_real(-180.0, 180.0);
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

std::string Faker::credit_card_number() {
    std::string digits = "4";
    while (digits.size() < 15) {
        digits += static_cast<char>('0' + number_int(0, 9));
    }
    digits += static_cast<char>('0' + luhn_check_digit(digits));
    return digits;
}

std::string Faker::currency_code() {
    return pick(rng_, kCurrencyCodes);
}

std::string Faker::iban() {
    return "DE" + zero_padded(number_int(0, 99), 2) +
        zero_padded(number_int(10000000, 99999999), 8) +
        zero_padded(number_int(100000000, 999999999), 9) +
        std::to_string(number_int(0, 9));
}

std::string Faker::product_name() {
    return pick(rng_, kProductAdjectives) + " " + pick(rng_, kProductNouns);
}

std::string Faker::product_category() {
    return pick(rng_, kProductCategories);
}

std::string Faker::price(double min, double max) {
    if (min > max) {
        throw std::invalid_argument("faker::Faker::price min must be <= max");
    }
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << number_real(min, max);
    return out.str();
}

std::string Faker::hex_string(std::size_t length) {
    constexpr std::string_view chars = "0123456789abcdef";
    std::string value;
    value.reserve(length);
    for (std::size_t i = 0; i < length; ++i) {
        value += chars[static_cast<std::size_t>(number_int(0, 15))];
    }
    return value;
}

std::string Faker::md5() {
    return hex_string(32);
}

std::string Faker::sha1() {
    return hex_string(40);
}

std::string Faker::sha256() {
    return hex_string(64);
}

std::string Faker::file_name() {
    return slug(word()) + "-" + std::to_string(number_int(1, 9999)) + "." + pick(rng_, kFileExtensions);
}

std::string Faker::file_path() {
    return "/" + slug(word()) + "/" + slug(word()) + "/" + file_name();
}

std::string Faker::mime_type() {
    return pick(rng_, kMimeTypes);
}

std::string Faker::semver() {
    return std::to_string(number_int(0, 9)) + "." +
        std::to_string(number_int(0, 20)) + "." +
        std::to_string(number_int(0, 99));
}

std::string Faker::hex_color() {
    return "#" + hex_string(6);
}

std::string Faker::rgb_color() {
    return "rgb(" + std::to_string(number_int(0, 255)) + ", " +
        std::to_string(number_int(0, 255)) + ", " +
        std::to_string(number_int(0, 255)) + ")";
}

Faker::Person Faker::person() { return Person(*this); }
Faker::Internet Faker::internet() { return Internet(*this); }
Faker::Location Faker::location() { return Location(*this); }
Faker::Company Faker::company() { return Company(*this); }
Faker::Lorem Faker::lorem() { return Lorem(*this); }
Faker::Finance Faker::finance() { return Finance(*this); }
Faker::Commerce Faker::commerce() { return Commerce(*this); }
Faker::Crypto Faker::crypto() { return Crypto(*this); }
Faker::System Faker::system() { return System(*this); }
Faker::Color Faker::color() { return Color(*this); }

Faker::Person::Person(Faker& faker) : faker_(&faker) {}
std::string Faker::Person::first_name() { return faker_->first_name(); }
std::string Faker::Person::last_name() { return faker_->last_name(); }
std::string Faker::Person::full_name() { return faker_->full_name(); }

Faker::Internet::Internet(Faker& faker) : faker_(&faker) {}
std::string Faker::Internet::username() { return faker_->username(); }
std::string Faker::Internet::email() { return faker_->email(); }
std::string Faker::Internet::domain_name() { return faker_->domain_name(); }
std::string Faker::Internet::url() { return faker_->url(); }
std::string Faker::Internet::ipv4() { return faker_->ipv4(); }
std::string Faker::Internet::ipv6() { return faker_->ipv6(); }
std::string Faker::Internet::mac_address() { return faker_->mac_address(); }
std::string Faker::Internet::password(int length) { return faker_->password(length); }
std::string Faker::Internet::user_agent() { return faker_->user_agent(); }

Faker::Location::Location(Faker& faker) : faker_(&faker) {}
std::string Faker::Location::street_address() { return faker_->street_address(); }
std::string Faker::Location::city() { return faker_->city(); }
std::string Faker::Location::country() { return faker_->country(); }
std::string Faker::Location::state() { return faker_->state(); }
std::string Faker::Location::zip_code() { return faker_->zip_code(); }
double Faker::Location::latitude() { return faker_->latitude(); }
double Faker::Location::longitude() { return faker_->longitude(); }

Faker::Company::Company(Faker& faker) : faker_(&faker) {}
std::string Faker::Company::name() { return faker_->company_name(); }
std::string Faker::Company::job_title() { return faker_->job_title(); }

Faker::Lorem::Lorem(Faker& faker) : faker_(&faker) {}
std::string Faker::Lorem::word() { return faker_->word(); }
std::string Faker::Lorem::sentence(int words) { return faker_->sentence(words); }
std::string Faker::Lorem::paragraph(int sentences) { return faker_->paragraph(sentences); }

Faker::Finance::Finance(Faker& faker) : faker_(&faker) {}
std::string Faker::Finance::credit_card_number() { return faker_->credit_card_number(); }
std::string Faker::Finance::currency_code() { return faker_->currency_code(); }
std::string Faker::Finance::iban() { return faker_->iban(); }

Faker::Commerce::Commerce(Faker& faker) : faker_(&faker) {}
std::string Faker::Commerce::product_name() { return faker_->product_name(); }
std::string Faker::Commerce::product_category() { return faker_->product_category(); }
std::string Faker::Commerce::price(double min, double max) { return faker_->price(min, max); }

Faker::Crypto::Crypto(Faker& faker) : faker_(&faker) {}
std::string Faker::Crypto::hex_string(std::size_t length) { return faker_->hex_string(length); }
std::string Faker::Crypto::md5() { return faker_->md5(); }
std::string Faker::Crypto::sha1() { return faker_->sha1(); }
std::string Faker::Crypto::sha256() { return faker_->sha256(); }

Faker::System::System(Faker& faker) : faker_(&faker) {}
std::string Faker::System::file_name() { return faker_->file_name(); }
std::string Faker::System::file_path() { return faker_->file_path(); }
std::string Faker::System::mime_type() { return faker_->mime_type(); }
std::string Faker::System::semver() { return faker_->semver(); }

Faker::Color::Color(Faker& faker) : faker_(&faker) {}
std::string Faker::Color::hex_color() { return faker_->hex_color(); }
std::string Faker::Color::rgb_color() { return faker_->rgb_color(); }

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
std::string ipv6() { return default_faker().ipv6(); }
std::string mac_address() { return default_faker().mac_address(); }
std::string password(int length) { return default_faker().password(length); }
std::string user_agent() { return default_faker().user_agent(); }
std::string phone_number() { return default_faker().phone_number(); }
std::string street_address() { return default_faker().street_address(); }
std::string city() { return default_faker().city(); }
std::string country() { return default_faker().country(); }
std::string state() { return default_faker().state(); }
std::string zip_code() { return default_faker().zip_code(); }
double latitude() { return default_faker().latitude(); }
double longitude() { return default_faker().longitude(); }
std::string company_name() { return default_faker().company_name(); }
std::string job_title() { return default_faker().job_title(); }
std::string word() { return default_faker().word(); }
std::string sentence(int words) { return default_faker().sentence(words); }
std::string paragraph(int sentences) { return default_faker().paragraph(sentences); }
std::string date_between(Faker::clock::time_point from, Faker::clock::time_point to) { return default_faker().date_between(from, to); }
std::string past_date(int days_back) { return default_faker().past_date(days_back); }
std::string future_date(int days_forward) { return default_faker().future_date(days_forward); }
std::string recent_date(int days_back) { return default_faker().recent_date(days_back); }
std::string credit_card_number() { return default_faker().credit_card_number(); }
std::string currency_code() { return default_faker().currency_code(); }
std::string iban() { return default_faker().iban(); }
std::string product_name() { return default_faker().product_name(); }
std::string product_category() { return default_faker().product_category(); }
std::string price(double min, double max) { return default_faker().price(min, max); }
std::string hex_string(std::size_t length) { return default_faker().hex_string(length); }
std::string md5() { return default_faker().md5(); }
std::string sha1() { return default_faker().sha1(); }
std::string sha256() { return default_faker().sha256(); }
std::string file_name() { return default_faker().file_name(); }
std::string file_path() { return default_faker().file_path(); }
std::string mime_type() { return default_faker().mime_type(); }
std::string semver() { return default_faker().semver(); }
std::string hex_color() { return default_faker().hex_color(); }
std::string rgb_color() { return default_faker().rgb_color(); }

} // namespace faker
