#include "faker/faker.hpp"

#include <array>
#include <charconv>
#include <cctype>
#include <cstdio>
#include <ctime>
#include <random>
#include <string_view>

namespace faker {
namespace {

constexpr std::array<std::string_view, 80> kFirstNames = {
    "Alex", "Avery", "Bailey", "Blair", "Cameron", "Casey", "Charlie", "Dakota",
    "Drew", "Elliot", "Emerson", "Finley", "Harper", "Hayden", "Jamie", "Jordan",
    "Kai", "Kendall", "Logan", "Morgan", "Parker", "Quinn", "Reese", "Riley",
    "River", "Rowan", "Sage", "Sam", "Sawyer", "Skyler", "Taylor", "Terry",
    "Toby", "Robin", "Noah", "Maya", "Iris", "Milo", "Nora", "Theo",
    "Ada", "Amelia", "Aria", "Atlas", "Beau", "Briar", "Callum", "Clara",
    "Cora", "Dylan", "Eden", "Elena", "Felix", "Freya", "Gavin", "Grace",
    "Hazel", "Henry", "Isla", "Jasper", "Juniper", "Lena", "Leo", "Luca",
    "Luna", "Maeve", "Miles", "Naomi", "Owen", "Paige", "Phoebe", "Remy",
    "Silas", "Sofia", "Stella", "Vera", "Wren", "Zoe", "Ezra", "Ivy",
};

constexpr std::array<std::string_view, 80> kLastNames = {
    "Anderson", "Baker", "Bennett", "Brooks", "Carter", "Clark", "Coleman", "Cooper",
    "Davis", "Diaz", "Edwards", "Evans", "Fisher", "Foster", "Garcia", "Gray",
    "Green", "Hall", "Harris", "Hayes", "Hill", "Howard", "Hughes", "Jackson",
    "James", "Johnson", "Kelly", "King", "Lewis", "Martin", "Miller", "Mitchell",
    "Moore", "Nelson", "Parker", "Perez", "Reed", "Roberts", "Smith", "Turner",
    "Adams", "Allen", "Bailey", "Bell", "Bryant", "Campbell", "Collins", "Cook",
    "Cox", "Cruz", "Flores", "Gomez", "Gonzalez", "Griffin", "Gutierrez", "Jenkins",
    "Jones", "Lee", "Long", "Lopez", "Martinez", "Morris", "Murphy", "Myers",
    "Nguyen", "Phillips", "Price", "Ramirez", "Rivera", "Ross", "Russell", "Sanchez",
    "Sanders", "Scott", "Stewart", "Thompson", "Walker", "Ward", "Watson", "Young",
};

static_assert(kFirstNames.size() == 80, "first name data must have exactly 80 entries");
static_assert(kLastNames.size() == 80, "last name data must have exactly 80 entries");

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

constexpr std::array<std::string_view, 69> kStates = {
    "California", "Texas", "Florida", "New York", "Pennsylvania", "Illinois",
    "Ohio", "Georgia", "North Carolina", "Michigan", "Washington", "Colorado",
    "Arizona", "Massachusetts", "Virginia", "Oregon", "Ontario", "Quebec",
    "British Columbia", "Alberta", "New South Wales", "Victoria", "Queensland",
    "Western Australia", "England", "Scotland", "Wales", "Northern Ireland",
    "Bavaria", "Saxony", "Hesse", "Brandenburg", "Berlin", "Hamburg",
    "North Rhine-Westphalia", "Baden-Wurttemberg", "Zurich", "Geneva", "Vaud",
    "Bern", "Vienna", "Tyrol", "Upper Austria", "Ile-de-France", "Normandy",
    "Brittany", "Catalonia", "Andalusia", "Madrid", "Lombardy", "Tuscany",
    "Lazio", "Masovia", "Lesser Poland", "South Holland", "North Holland",
    "Zealand", "Stockholm", "Skane", "Oslo", "Akershus", "Capital Region",
    "Uusimaa", "Prague", "South Moravian", "Tokyo", "Osaka", "Seoul", "Busan",
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

constexpr std::array<std::string_view, 16> kTlds = {
    "com", "net", "org", "dev", "io", "app", "cloud", "tech", "example", "test",
    "de", "it", "gov", "tk", "ai", "tv",
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
std::string_view pick_view(std::mt19937_64& rng, const std::array<std::string_view, N>& values) {
    std::uniform_int_distribution<std::size_t> dist(0, values.size() - 1);
    return values[dist(rng)];
}

template <std::size_t N>
std::string pick(std::mt19937_64& rng, const std::array<std::string_view, N>& values) {
    return std::string(pick_view(rng, values));
}

int random_int(std::mt19937_64& rng, int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

void append_int(std::string& out, int value) {
    char buffer[32]{};
    const auto result = std::to_chars(buffer, buffer + sizeof(buffer), value);
    out.append(buffer, result.ptr);
}

void append_zero_padded(std::string& out, int value, int width) {
    char buffer[32]{};
    const auto result = std::to_chars(buffer, buffer + sizeof(buffer), value);
    const auto digits = static_cast<int>(result.ptr - buffer);
    if (digits < width) {
        out.append(static_cast<std::size_t>(width - digits), '0');
    }
    out.append(buffer, result.ptr);
}

void append_lower_ascii(std::string& out, std::string_view value) {
    for (const auto ch : value) {
        out += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
}

void append_compact_lower_ascii(std::string& out, std::string_view value) {
    for (const auto ch : value) {
        if (std::isspace(static_cast<unsigned char>(ch)) == 0) {
            out += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }
    }
}

void append_slug(std::string& out, std::string_view value) {
    for (const auto ch : value) {
        out += ch == ' ' ? '-' : static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
}

void append_hex_byte(std::string& out, int value) {
    constexpr char kHex[] = "0123456789abcdef";
    out += kHex[(value >> 4) & 0x0f];
    out += kHex[value & 0x0f];
}

void append_hex_word(std::string& out, int value) {
    append_hex_byte(out, (value >> 8) & 0xff);
    append_hex_byte(out, value & 0xff);
}

void append_hex_chars(std::string& out, std::mt19937_64& rng, std::size_t length) {
    constexpr std::string_view chars = "0123456789abcdef";
    std::uniform_int_distribution<int> hex_dist(0, 15);
    for (std::size_t i = 0; i < length; ++i) {
        out += chars[static_cast<std::size_t>(hex_dist(rng))];
    }
}

void append_domain_name(std::string& out, std::mt19937_64& rng) {
    append_compact_lower_ascii(out, pick_view(rng, kCompanyPrefixes));
    out += '-';
    append_compact_lower_ascii(out, pick_view(rng, kCompanyNouns));
    out += '.';
    out += pick_view(rng, kTlds);
}

void append_username(std::string& out, std::mt19937_64& rng) {
    append_lower_ascii(out, pick_view(rng, kFirstNames));
    out += '.';
    append_lower_ascii(out, pick_view(rng, kLastNames));
    append_int(out, random_int(rng, 10, 9999));
}

void append_file_name(std::string& out, std::mt19937_64& rng) {
    append_slug(out, pick_view(rng, kWords));
    out += '-';
    append_int(out, random_int(rng, 1, 9999));
    out += '.';
    out += pick_view(rng, kFileExtensions);
}

std::string format_date(Faker::clock::time_point time) {
    const auto raw = Faker::clock::to_time_t(time);
    std::tm tm_value{};
#if defined(_WIN32)
    gmtime_s(&tm_value, &raw);
#else
    gmtime_r(&raw, &tm_value);
#endif

    std::string out;
    out.reserve(10);
    append_int(out, tm_value.tm_year + 1900);
    out += '-';
    append_zero_padded(out, tm_value.tm_mon + 1, 2);
    out += '-';
    append_zero_padded(out, tm_value.tm_mday, 2);
    return out;
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
    return random_int(rng_, min, max);
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
    std::uniform_int_distribution<int> byte_dist(0, 255);
    for (auto& byte : bytes) {
        byte = static_cast<unsigned char>(byte_dist(rng_));
    }
    bytes[6] = static_cast<unsigned char>((bytes[6] & 0x0fU) | 0x40U);
    bytes[8] = static_cast<unsigned char>((bytes[8] & 0x3fU) | 0x80U);

    std::string out;
    out.reserve(36);
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        append_hex_byte(out, bytes[i]);
        if (i == 3 || i == 5 || i == 7 || i == 9) {
            out += '-';
        }
    }
    return out;
}

std::string Faker::first_name() {
    return pick(rng_, kFirstNames);
}

std::string Faker::last_name() {
    return pick(rng_, kLastNames);
}

std::string Faker::full_name() {
    const auto first = pick_view(rng_, kFirstNames);
    const auto last = pick_view(rng_, kLastNames);
    std::string out;
    out.reserve(first.size() + 1 + last.size());
    out += first;
    out += ' ';
    out += last;
    return out;
}

std::string Faker::username() {
    std::string out;
    out.reserve(32);
    append_username(out, rng_);
    return out;
}

std::string Faker::email() {
    std::string out;
    out.reserve(64);
    append_username(out, rng_);
    out += '@';
    append_domain_name(out, rng_);
    return out;
}

std::string Faker::domain_name() {
    std::string out;
    out.reserve(32);
    append_domain_name(out, rng_);
    return out;
}

std::string Faker::url() {
    std::string out;
    out.reserve(48);
    out += "https://www.";
    append_domain_name(out, rng_);
    return out;
}

std::string Faker::ipv4() {
    std::string out;
    out.reserve(15);
    append_int(out, random_int(rng_, 1, 223));
    out += '.';
    append_int(out, random_int(rng_, 0, 255));
    out += '.';
    append_int(out, random_int(rng_, 0, 255));
    out += '.';
    append_int(out, random_int(rng_, 1, 254));
    return out;
}

std::string Faker::ipv6() {
    std::uniform_int_distribution<int> word_dist(0, 65535);
    std::string out;
    out.reserve(39);
    for (int i = 0; i < 8; ++i) {
        if (i != 0) {
            out += ':';
        }
        append_hex_word(out, word_dist(rng_));
    }
    return out;
}

std::string Faker::mac_address() {
    std::uniform_int_distribution<int> byte_dist(0, 255);
    std::string out;
    out.reserve(17);
    for (int i = 0; i < 6; ++i) {
        if (i != 0) {
            out += ':';
        }
        append_hex_byte(out, byte_dist(rng_));
    }
    return out;
}

std::string Faker::password(int length) {
    if (length <= 0) {
        throw std::invalid_argument("faker::Faker::password length must be positive");
    }

    constexpr std::string_view chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    std::uniform_int_distribution<std::size_t> char_dist(0, chars.size() - 1);
    std::string value;
    value.reserve(static_cast<std::size_t>(length));
    for (int i = 0; i < length; ++i) {
        value += chars[char_dist(rng_)];
    }
    return value;
}

std::string Faker::user_agent() {
    return pick(rng_, kUserAgents);
}

std::string Faker::phone_number() {
    std::string out;
    out.reserve(15);
    out += "+1-";
    append_int(out, random_int(rng_, 200, 999));
    out += '-';
    append_int(out, random_int(rng_, 200, 999));
    out += '-';
    append_zero_padded(out, random_int(rng_, 0, 9999), 4);
    return out;
}

std::string Faker::street_address() {
    const auto street = pick_view(rng_, kStreetNames);
    std::string out;
    out.reserve(6 + street.size() + 7);
    append_int(out, random_int(rng_, 1, 9999));
    out += ' ';
    out += street;
    out += " Street";
    return out;
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
    std::string out;
    out.reserve(5);
    append_zero_padded(out, random_int(rng_, 0, 99999), 5);
    return out;
}

double Faker::latitude() {
    return number_real(-90.0, 90.0);
}

double Faker::longitude() {
    return number_real(-180.0, 180.0);
}

std::string Faker::company_name() {
    const auto prefix = pick_view(rng_, kCompanyPrefixes);
    const auto noun = pick_view(rng_, kCompanyNouns);
    std::string out;
    out.reserve(prefix.size() + 1 + noun.size());
    out += prefix;
    out += ' ';
    out += noun;
    return out;
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
    value.reserve(static_cast<std::size_t>(words * 8 + 1));
    for (int i = 0; i < words; ++i) {
        if (i != 0) {
            value += ' ';
        }
        const auto picked = pick_view(rng_, kWords);
        if (i == 0 && !picked.empty()) {
            value += static_cast<char>(std::toupper(static_cast<unsigned char>(picked.front())));
            value.append(picked.data() + 1, picked.size() - 1);
        } else {
            value += picked;
        }
    }
    value += '.';
    return value;
}

std::string Faker::paragraph(int sentences) {
    if (sentences <= 0) {
        throw std::invalid_argument("faker::Faker::paragraph sentences must be positive");
    }

    std::string value;
    value.reserve(static_cast<std::size_t>(sentences * 80));
    for (int i = 0; i < sentences; ++i) {
        if (i != 0) {
            value += ' ';
        }
        const auto words = random_int(rng_, 6, 14);
        for (int j = 0; j < words; ++j) {
            if (j != 0) {
                value += ' ';
            }
            const auto picked = pick_view(rng_, kWords);
            if (j == 0 && !picked.empty()) {
                value += static_cast<char>(std::toupper(static_cast<unsigned char>(picked.front())));
                value.append(picked.data() + 1, picked.size() - 1);
            } else {
                value += picked;
            }
        }
        value += '.';
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
        digits += static_cast<char>('0' + random_int(rng_, 0, 9));
    }
    digits += static_cast<char>('0' + luhn_check_digit(digits));
    return digits;
}

std::string Faker::currency_code() {
    return pick(rng_, kCurrencyCodes);
}

std::string Faker::iban() {
    std::string out;
    out.reserve(22);
    out += "DE";
    append_zero_padded(out, random_int(rng_, 0, 99), 2);
    append_zero_padded(out, random_int(rng_, 10000000, 99999999), 8);
    append_zero_padded(out, random_int(rng_, 100000000, 999999999), 9);
    append_int(out, random_int(rng_, 0, 9));
    return out;
}

std::string Faker::product_name() {
    const auto adjective = pick_view(rng_, kProductAdjectives);
    const auto noun = pick_view(rng_, kProductNouns);
    std::string out;
    out.reserve(adjective.size() + 1 + noun.size());
    out += adjective;
    out += ' ';
    out += noun;
    return out;
}

std::string Faker::product_category() {
    return pick(rng_, kProductCategories);
}

std::string Faker::price(double min, double max) {
    if (min > max) {
        throw std::invalid_argument("faker::Faker::price min must be <= max");
    }
    char buffer[32]{};
    std::snprintf(buffer, sizeof(buffer), "%.2f", number_real(min, max));
    return std::string(buffer);
}

std::string Faker::hex_string(std::size_t length) {
    std::string value;
    value.reserve(length);
    append_hex_chars(value, rng_, length);
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
    std::string out;
    out.reserve(20);
    append_file_name(out, rng_);
    return out;
}

std::string Faker::file_path() {
    std::string out;
    out.reserve(48);
    out += '/';
    append_slug(out, pick_view(rng_, kWords));
    out += '/';
    append_slug(out, pick_view(rng_, kWords));
    out += '/';
    append_file_name(out, rng_);
    return out;
}

std::string Faker::mime_type() {
    return pick(rng_, kMimeTypes);
}

std::string Faker::semver() {
    std::string out;
    out.reserve(8);
    append_int(out, random_int(rng_, 0, 9));
    out += '.';
    append_int(out, random_int(rng_, 0, 20));
    out += '.';
    append_int(out, random_int(rng_, 0, 99));
    return out;
}

std::string Faker::hex_color() {
    std::string out;
    out.reserve(7);
    out += '#';
    append_hex_chars(out, rng_, 6);
    return out;
}

std::string Faker::rgb_color() {
    std::string out;
    out.reserve(18);
    out += "rgb(";
    append_int(out, random_int(rng_, 0, 255));
    out += ", ";
    append_int(out, random_int(rng_, 0, 255));
    out += ", ";
    append_int(out, random_int(rng_, 0, 255));
    out += ')';
    return out;
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
