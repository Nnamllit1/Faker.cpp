#include <faker/faker.hpp>

#include <cassert>
#include <cctype>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "test failed: " << message << '\n';
        std::exit(1);
    }
}

bool is_hex_string(const std::string& value) {
    for (const auto ch : value) {
        if (!std::isxdigit(static_cast<unsigned char>(ch)) || std::isupper(static_cast<unsigned char>(ch))) {
            return false;
        }
    }
    return true;
}

bool is_digits(const std::string& value) {
    for (const auto ch : value) {
        if (!std::isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
    }
    return true;
}

bool luhn_valid(const std::string& digits) {
    int sum = 0;
    bool double_digit = false;
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
    return sum % 10 == 0;
}

void test_deterministic_seed() {
    faker::Faker a(1234);
    faker::Faker b(1234);

    require(a.full_name() == b.full_name(), "same seed should produce same names");
    require(a.email() == b.email(), "same seed should produce same emails");
    require(a.uuid_v4() == b.uuid_v4(), "same seed should produce same UUIDs");
}

void test_uuid_shape() {
    faker::Faker fake(7);
    const auto id = fake.uuid_v4();

    require(id.size() == 36, "uuid length");
    require(id[8] == '-' && id[13] == '-' && id[18] == '-' && id[23] == '-', "uuid separators");
    require(id[14] == '4', "uuid version");
    require(id[19] == '8' || id[19] == '9' || id[19] == 'a' || id[19] == 'b', "uuid variant");
}

void test_ranges_and_helpers() {
    faker::Faker fake(99);

    for (int i = 0; i < 100; ++i) {
        const auto n = fake.number_int(10, 20);
        require(n >= 10 && n <= 20, "integer range");

        const auto d = fake.number_real(1.5, 2.5);
        require(d >= 1.5 && d <= 2.5, "real range");
    }

    const std::vector<std::string> values = {"red", "green", "blue"};
    const auto& picked = fake.choice(values);
    require(!picked.empty(), "choice result");
}

void test_category_smoke() {
    faker::Faker fake(2026);

    require(!fake.first_name().empty(), "first_name");
    require(!fake.last_name().empty(), "last_name");
    require(fake.full_name().find(' ') != std::string::npos, "full_name");
    require(!fake.name_prefix().empty(), "name_prefix");
    require(!fake.name_suffix().empty(), "name_suffix");
    require(!fake.middle_name().empty(), "middle_name");
    require(fake.email().find('@') != std::string::npos, "email");
    require(fake.url().find("https://") == 0, "url");
    require(!fake.ipv4().empty(), "ipv4");
    require(!fake.phone_number().empty(), "phone_number");
    require(!fake.building_number().empty(), "building_number");
    require(!fake.street_name().empty(), "street_name");
    require(!fake.street_address().empty(), "street_address");
    require(!fake.secondary_address().empty(), "secondary_address");
    require(fake.full_address().find(", ") != std::string::npos, "full_address formatting");
    require(fake.mailing_address().find('\n') != std::string::npos, "mailing_address lines");
    require(!fake.city().empty(), "city");
    require(!fake.country().empty(), "country");
    require(!fake.company_name().empty(), "company_name");
    require(!fake.job_title().empty(), "job_title");
    require(!fake.word().empty(), "word");
    require(fake.sentence().back() == '.', "sentence punctuation");
    require(!fake.paragraph().empty(), "paragraph");
    require(fake.recent_date().size() == 10, "recent date yyyy-mm-dd");
    require(fake.past_date().size() == 10, "past date yyyy-mm-dd");
    require(fake.future_date().size() == 10, "future date yyyy-mm-dd");
}

void test_expanded_categories() {
    faker::Faker fake(404);

    const auto mac = fake.mac_address();
    require(mac.size() == 17 && mac[2] == ':' && mac[5] == ':' && mac[8] == ':' &&
        mac[11] == ':' && mac[14] == ':', "mac address shape");

    const auto ipv6 = fake.ipv6();
    require(ipv6.find(':') != std::string::npos, "ipv6 contains separators");

    require(fake.password(24).size() == 24, "password length");
    require(!fake.user_agent().empty(), "user agent");
    require(!fake.state().empty(), "state");
    require(fake.zip_code().size() == 5 && is_digits(fake.zip_code()), "zip code");
    require(fake.postal_code().size() == 5 && is_digits(fake.postal_code()), "postal code");
    require(fake.latitude() >= -90.0 && fake.latitude() <= 90.0, "latitude range");
    require(fake.longitude() >= -180.0 && fake.longitude() <= 180.0, "longitude range");

    const auto card = fake.credit_card_number();
    require(card.size() == 16 && is_digits(card) && luhn_valid(card), "credit card shape");
    require(fake.currency_code().size() == 3, "currency code");
    require(fake.iban().size() == 22, "iban shape");
    require(!fake.product_name().empty(), "product name");
    require(!fake.product_category().empty(), "product category");
    require(fake.price().find('.') != std::string::npos, "price decimal");

    require(fake.md5().size() == 32 && is_hex_string(fake.md5()), "md5 shape");
    require(fake.sha1().size() == 40 && is_hex_string(fake.sha1()), "sha1 shape");
    require(fake.sha256().size() == 64 && is_hex_string(fake.sha256()), "sha256 shape");
    require(fake.hex_string(12).size() == 12, "hex string length");

    require(fake.file_name().find('.') != std::string::npos, "file name extension");
    require(fake.file_path().find('/') != std::string::npos, "file path separator");
    require(fake.mime_type().find('/') != std::string::npos, "mime type separator");
    require(fake.semver().find('.') != std::string::npos, "semver");
    require(fake.hex_color().size() == 7 && fake.hex_color()[0] == '#', "hex color");
    require(fake.rgb_color().find("rgb(") == 0, "rgb color");
}

void test_category_aliases() {
    faker::Faker fake(8080);

    require(!fake.person().full_name().empty(), "person alias");
    require(!fake.person().name_prefix().empty(), "person prefix alias");
    require(!fake.person().name_suffix().empty(), "person suffix alias");
    require(!fake.person().middle_name().empty(), "person middle alias");
    require(fake.internet().email().find('@') != std::string::npos, "internet alias");
    require(!fake.location().building_number().empty(), "location building alias");
    require(!fake.location().street_name().empty(), "location street alias");
    require(!fake.location().secondary_address().empty(), "location secondary alias");
    require(fake.location().full_address().find(", ") != std::string::npos, "location full address alias");
    require(fake.location().mailing_address().find('\n') != std::string::npos, "location mailing address alias");
    require(fake.location().postal_code().size() == 5, "location postal alias");
    require(!fake.location().city().empty(), "location city alias");
    require(!fake.company().name().empty(), "company alias");
    require(fake.lorem().sentence().back() == '.', "lorem alias");
    require(luhn_valid(fake.finance().credit_card_number()), "finance alias");
    require(!fake.commerce().product_name().empty(), "commerce alias");
    require(fake.crypto().sha256().size() == 64, "crypto alias");
    require(!fake.system().file_name().empty(), "system alias");
    require(fake.color().hex_color()[0] == '#', "color alias");
}

void test_namespace_helpers() {
    faker::seed(9090);
    require(!faker::name_prefix().empty(), "namespace name_prefix");
    require(!faker::name_suffix().empty(), "namespace name_suffix");
    require(!faker::middle_name().empty(), "namespace middle_name");
    require(!faker::building_number().empty(), "namespace building_number");
    require(!faker::street_name().empty(), "namespace street_name");
    require(!faker::secondary_address().empty(), "namespace secondary_address");
    require(faker::full_address().find(", ") != std::string::npos, "namespace full_address");
    require(faker::mailing_address().find('\n') != std::string::npos, "namespace mailing_address");
    require(faker::postal_code().size() == 5 && is_digits(faker::postal_code()), "namespace postal_code");
}

void test_validation() {
    faker::Faker fake(1);

    bool threw = false;
    try {
        fake.number_int(2, 1);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require(threw, "invalid integer range throws");

    threw = false;
    try {
        fake.boolean(2.0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require(threw, "invalid probability throws");

    threw = false;
    try {
        fake.password(0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require(threw, "invalid password length throws");

    threw = false;
    try {
        fake.price(10.0, 1.0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require(threw, "invalid price range throws");
}

} // namespace

int main() {
    test_deterministic_seed();
    test_uuid_shape();
    test_ranges_and_helpers();
    test_category_smoke();
    test_expanded_categories();
    test_category_aliases();
    test_namespace_helpers();
    test_validation();

    std::cout << "faker_tests passed\n";
    return 0;
}
