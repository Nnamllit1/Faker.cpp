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
    require(fake.email().find('@') != std::string::npos, "email");
    require(fake.url().find("https://") == 0, "url");
    require(!fake.ipv4().empty(), "ipv4");
    require(!fake.phone_number().empty(), "phone_number");
    require(!fake.street_address().empty(), "street_address");
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
}

} // namespace

int main() {
    test_deterministic_seed();
    test_uuid_shape();
    test_ranges_and_helpers();
    test_category_smoke();
    test_validation();

    std::cout << "faker_tests passed\n";
    return 0;
}

