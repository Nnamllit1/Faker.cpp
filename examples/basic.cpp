#include <faker/faker.hpp>

#include <iostream>

int main() {
    faker::Faker fake(42);

    std::cout << "name: " << fake.full_name() << '\n';
    std::cout << "email: " << fake.email() << '\n';
    std::cout << "company: " << fake.company_name() << '\n';
    std::cout << "address: " << fake.street_address() << ", " << fake.city() << '\n';
    std::cout << "uuid: " << fake.uuid_v4() << '\n';
    std::cout << "sha256: " << fake.crypto().sha256() << '\n';
    std::cout << "price: " << fake.commerce().price(10.0, 80.0) << '\n';
    std::cout << "color: " << fake.color().hex_color() << '\n';
    std::cout << "sentence: " << fake.sentence() << '\n';
}
