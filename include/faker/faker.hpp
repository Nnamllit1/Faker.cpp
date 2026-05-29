#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <string>

namespace faker {

/**
 * Stateful fake data generator.
 *
 * Faker owns one random engine. Create an instance with a fixed seed when tests
 * need reproducible output, or use the namespace-level helpers for quick scripts.
 */
class Faker {
public:
    using clock = std::chrono::system_clock;

    class Person;
    class Internet;
    class Location;
    class Company;
    class Lorem;
    class Finance;
    class Commerce;
    class Crypto;
    class System;
    class Color;

    /** Creates a generator seeded from std::random_device. */
    Faker();

    /** Creates a generator with deterministic output for the given seed. */
    explicit Faker(std::uint64_t seed);

    /** Reseeds this generator. */
    void seed(std::uint64_t value);

    /** Returns true with the given probability, from 0.0 to 1.0. */
    bool boolean(double probability = 0.5);

    /** Returns a random integer in the inclusive [min, max] range. */
    int number_int(int min, int max);

    /** Returns a random double in the [min, max] range. */
    double number_real(double min, double max);

    /** Returns a random RFC 4122 version 4 UUID string. */
    std::string uuid_v4();

    /** Returns a first name. */
    std::string first_name();

    /** Returns a last name. */
    std::string last_name();

    /** Returns a first and last name. */
    std::string full_name();

    /** Returns a name prefix such as Dr. or Ms. */
    std::string name_prefix();

    /** Returns a name suffix such as Jr. or III. */
    std::string name_suffix();

    /** Returns a middle name. */
    std::string middle_name();

    /** Returns an internet-style username. */
    std::string username();

    /** Returns an email address. */
    std::string email();

    /** Returns a domain name. */
    std::string domain_name();

    /** Returns an HTTPS URL. */
    std::string url();

    /** Returns an IPv4 address. */
    std::string ipv4();

    /** Returns an IPv6 address. */
    std::string ipv6();

    /** Returns a MAC address. */
    std::string mac_address();

    /** Returns a fake password of the requested length. */
    std::string password(int length = 16);

    /** Returns a common browser user-agent string. */
    std::string user_agent();

    /** Returns a phone number. */
    std::string phone_number();

    /** Returns a building number. */
    std::string building_number();

    /** Returns a street name. */
    std::string street_name();

    /** Returns a street address. */
    std::string street_address();

    /** Returns a secondary address line such as an apartment or suite. */
    std::string secondary_address();

    /** Returns a city name. */
    std::string city();

    /** Returns a country name. */
    std::string country();

    /** Returns a state or region name. */
    std::string state();

    /** Returns a postal code. */
    std::string zip_code();

    /** Returns a postal code. */
    std::string postal_code();

    /** Returns a latitude in degrees. */
    double latitude();

    /** Returns a longitude in degrees. */
    double longitude();

    /** Returns a company name. */
    std::string company_name();

    /** Returns a job title. */
    std::string job_title();

    /** Returns a lorem-style word. */
    std::string word();

    /** Returns a lorem-style sentence with the requested word count. */
    std::string sentence(int words = 8);

    /** Returns a lorem-style paragraph with the requested sentence count. */
    std::string paragraph(int sentences = 3);

    /** Returns a date formatted as YYYY-MM-DD within the inclusive range. */
    std::string date_between(clock::time_point from, clock::time_point to);

    /** Returns a date formatted as YYYY-MM-DD between now and days_back days ago. */
    std::string past_date(int days_back = 365);

    /** Returns a date formatted as YYYY-MM-DD between now and days_forward days from now. */
    std::string future_date(int days_forward = 365);

    /** Returns a recent date formatted as YYYY-MM-DD. */
    std::string recent_date(int days_back = 7);

    /** Returns a Luhn-valid fake credit-card-like number for tests. */
    std::string credit_card_number();

    /** Returns an ISO-style three-letter currency code. */
    std::string currency_code();

    /** Returns a fake IBAN-shaped string. */
    std::string iban();

    /** Returns a product name. */
    std::string product_name();

    /** Returns a product category. */
    std::string product_category();

    /** Returns a price string with two decimal places. */
    std::string price(double min = 1.0, double max = 500.0);

    /** Returns random lowercase hexadecimal text with the requested character count. */
    std::string hex_string(std::size_t length = 32);

    /** Returns a fake MD5-shaped hash. This is not cryptographic. */
    std::string md5();

    /** Returns a fake SHA-1-shaped hash. This is not cryptographic. */
    std::string sha1();

    /** Returns a fake SHA-256-shaped hash. This is not cryptographic. */
    std::string sha256();

    /** Returns a file name with an extension. */
    std::string file_name();

    /** Returns a slash-separated fake path. */
    std::string file_path();

    /** Returns a MIME type. */
    std::string mime_type();

    /** Returns a semantic version string. */
    std::string semver();

    /** Returns a CSS-style hexadecimal color. */
    std::string hex_color();

    /** Returns a CSS-style rgb(r, g, b) color. */
    std::string rgb_color();

    /** Returns a random element from a non-empty indexable container. */
    template <typename Container>
    const typename Container::value_type& choice(const Container& values) {
        if (values.empty()) {
            throw std::invalid_argument("faker::Faker::choice requires a non-empty container");
        }
        return values[static_cast<typename Container::size_type>(number_int(0, static_cast<int>(values.size() - 1)))];
    }

    /** Person category alias. */
    Person person();

    /** Internet category alias. */
    Internet internet();

    /** Location category alias. */
    Location location();

    /** Company category alias. */
    Company company();

    /** Lorem category alias. */
    Lorem lorem();

    /** Finance category alias. */
    Finance finance();

    /** Commerce category alias. */
    Commerce commerce();

    /** Crypto category alias for fake hashes and hex strings. */
    Crypto crypto();

    /** System category alias for file, MIME, and version strings. */
    System system();

    /** Color category alias. */
    Color color();

private:
    std::mt19937_64 rng_;
};

/** Person category proxy. */
class Faker::Person {
public:
    explicit Person(Faker& faker);
    std::string first_name();
    std::string last_name();
    std::string full_name();
    std::string name_prefix();
    std::string name_suffix();
    std::string middle_name();

private:
    Faker* faker_;
};

/** Internet category proxy. */
class Faker::Internet {
public:
    explicit Internet(Faker& faker);
    std::string username();
    std::string email();
    std::string domain_name();
    std::string url();
    std::string ipv4();
    std::string ipv6();
    std::string mac_address();
    std::string password(int length = 16);
    std::string user_agent();

private:
    Faker* faker_;
};

/** Location category proxy. */
class Faker::Location {
public:
    explicit Location(Faker& faker);
    std::string building_number();
    std::string street_name();
    std::string street_address();
    std::string secondary_address();
    std::string city();
    std::string country();
    std::string state();
    std::string zip_code();
    std::string postal_code();
    double latitude();
    double longitude();

private:
    Faker* faker_;
};

/** Company category proxy. */
class Faker::Company {
public:
    explicit Company(Faker& faker);
    std::string name();
    std::string job_title();

private:
    Faker* faker_;
};

/** Lorem category proxy. */
class Faker::Lorem {
public:
    explicit Lorem(Faker& faker);
    std::string word();
    std::string sentence(int words = 8);
    std::string paragraph(int sentences = 3);

private:
    Faker* faker_;
};

/** Finance category proxy. */
class Faker::Finance {
public:
    explicit Finance(Faker& faker);
    std::string credit_card_number();
    std::string currency_code();
    std::string iban();

private:
    Faker* faker_;
};

/** Commerce category proxy. */
class Faker::Commerce {
public:
    explicit Commerce(Faker& faker);
    std::string product_name();
    std::string product_category();
    std::string price(double min = 1.0, double max = 500.0);

private:
    Faker* faker_;
};

/** Crypto category proxy for fake hashes and random-looking hex values. */
class Faker::Crypto {
public:
    explicit Crypto(Faker& faker);
    std::string hex_string(std::size_t length = 32);
    std::string md5();
    std::string sha1();
    std::string sha256();

private:
    Faker* faker_;
};

/** System category proxy. */
class Faker::System {
public:
    explicit System(Faker& faker);
    std::string file_name();
    std::string file_path();
    std::string mime_type();
    std::string semver();

private:
    Faker* faker_;
};

/** Color category proxy. */
class Faker::Color {
public:
    explicit Color(Faker& faker);
    std::string hex_color();
    std::string rgb_color();

private:
    Faker* faker_;
};

/** Returns this thread's default generator. */
Faker& default_faker();

/** Reseeds this thread's default generator. */
void seed(std::uint64_t value);

bool boolean(double probability = 0.5);
int number_int(int min, int max);
double number_real(double min, double max);
std::string uuid_v4();

std::string first_name();
std::string last_name();
std::string full_name();
std::string name_prefix();
std::string name_suffix();
std::string middle_name();
std::string username();
std::string email();
std::string domain_name();
std::string url();
std::string ipv4();
std::string ipv6();
std::string mac_address();
std::string password(int length = 16);
std::string user_agent();

std::string phone_number();
std::string building_number();
std::string street_name();
std::string street_address();
std::string secondary_address();
std::string city();
std::string country();
std::string state();
std::string zip_code();
std::string postal_code();
double latitude();
double longitude();
std::string company_name();
std::string job_title();

std::string word();
std::string sentence(int words = 8);
std::string paragraph(int sentences = 3);

std::string date_between(Faker::clock::time_point from, Faker::clock::time_point to);
std::string past_date(int days_back = 365);
std::string future_date(int days_forward = 365);
std::string recent_date(int days_back = 7);

std::string credit_card_number();
std::string currency_code();
std::string iban();
std::string product_name();
std::string product_category();
std::string price(double min = 1.0, double max = 500.0);
std::string hex_string(std::size_t length = 32);
std::string md5();
std::string sha1();
std::string sha256();
std::string file_name();
std::string file_path();
std::string mime_type();
std::string semver();
std::string hex_color();
std::string rgb_color();

} // namespace faker

/**
 * Define FAKER_CPP_IMPLEMENTATION in exactly one translation unit to include
 * the implementation through the header instead of compiling src/faker.cpp.
 */
#ifdef FAKER_CPP_IMPLEMENTATION
#include "../../src/faker.cpp"
#endif
