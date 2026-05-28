#pragma once

#include <chrono>
#include <cstdint>
#include <initializer_list>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace faker {

class Faker {
public:
    using clock = std::chrono::system_clock;

    Faker();
    explicit Faker(std::uint64_t seed);

    void seed(std::uint64_t value);

    bool boolean(double probability = 0.5);
    int number_int(int min, int max);
    double number_real(double min, double max);
    std::string uuid_v4();

    std::string first_name();
    std::string last_name();
    std::string full_name();
    std::string username();
    std::string email();
    std::string domain_name();
    std::string url();
    std::string ipv4();

    std::string phone_number();
    std::string street_address();
    std::string city();
    std::string country();
    std::string company_name();
    std::string job_title();

    std::string word();
    std::string sentence(int words = 8);
    std::string paragraph(int sentences = 3);

    std::string date_between(clock::time_point from, clock::time_point to);
    std::string past_date(int days_back = 365);
    std::string future_date(int days_forward = 365);
    std::string recent_date(int days_back = 7);

    template <typename Container>
    const typename Container::value_type& choice(const Container& values) {
        if (values.empty()) {
            throw std::invalid_argument("faker::Faker::choice requires a non-empty container");
        }
        return values[static_cast<typename Container::size_type>(number_int(0, static_cast<int>(values.size() - 1)))];
    }

private:
    std::mt19937_64 rng_;
};

Faker& default_faker();
void seed(std::uint64_t value);

bool boolean(double probability = 0.5);
int number_int(int min, int max);
double number_real(double min, double max);
std::string uuid_v4();

std::string first_name();
std::string last_name();
std::string full_name();
std::string username();
std::string email();
std::string domain_name();
std::string url();
std::string ipv4();

std::string phone_number();
std::string street_address();
std::string city();
std::string country();
std::string company_name();
std::string job_title();

std::string word();
std::string sentence(int words = 8);
std::string paragraph(int sentences = 3);

std::string date_between(Faker::clock::time_point from, Faker::clock::time_point to);
std::string past_date(int days_back = 365);
std::string future_date(int days_forward = 365);
std::string recent_date(int days_back = 7);

} // namespace faker

#ifdef FAKER_CPP_IMPLEMENTATION
#include "../../src/faker.cpp"
#endif
