# API

## Generator

`faker::Faker` owns its random engine. Construct it with a seed when tests need repeatable output:

```cpp
faker::Faker fake(1234);
```

Use namespace-level helpers for quick scripts. They use a thread-local default generator:

```cpp
faker::seed(1234);
auto email = faker::email();
auto animal = faker::animal();
```

## Person

```cpp
fake.first_name();
fake.last_name();
fake.full_name();
fake.name_prefix();
fake.name_suffix();
fake.middle_name();

fake.person().full_name();
```

## Internet

```cpp
fake.username();
fake.email();
fake.domain_name();
fake.url();
fake.ipv4();
fake.ipv6();
fake.mac_address();
fake.password();
fake.user_agent();

fake.internet().email();
```

## Location

```cpp
fake.phone_number();
fake.building_number();
fake.street_name();
fake.street_address();
fake.secondary_address();
fake.full_address();
fake.mailing_address();
fake.city();
fake.country();
fake.state();
fake.zip_code();
fake.postal_code();
fake.latitude();
fake.longitude();

fake.location().mailing_address();
```

`full_address()` returns a one-line address. `mailing_address()` returns a multi-line address suitable for fixtures that need a complete address block.

## Company And Commerce

```cpp
fake.company_name();
fake.job_title();
fake.product_name();
fake.product_category();
fake.price();

fake.company().name();
fake.commerce().product_name();
```

## Text And Primitives

```cpp
fake.word();
fake.sentence();
fake.paragraph();
fake.boolean();
fake.number_int(1, 10);
fake.number_real(0.0, 1.0);
fake.uuid_v4();
```

## Dates

Dates return `YYYY-MM-DD` strings:

```cpp
fake.past_date();
fake.future_date();
fake.recent_date();
```

Use `date_between(from, to)` when the test needs an explicit range.

## Finance

```cpp
fake.credit_card_number();
fake.currency_code();
fake.iban();

fake.finance().credit_card_number();
```

`credit_card_number()` returns a Luhn-valid fake number for tests. It is not connected to real card accounts.

## Crypto-Shaped Test Data

```cpp
fake.hex_string();
fake.md5();
fake.sha1();
fake.sha256();

fake.crypto().sha256();
```

These values are random-looking fake data for tests. They are not cryptographic primitives.

## System And Colors

```cpp
fake.file_name();
fake.file_path();
fake.mime_type();
fake.semver();
fake.hex_color();
fake.rgb_color();

fake.system().file_name();
fake.color().hex_color();
```

## Animals

```cpp
fake.animal();
fake.animal_type();
fake.mammal();
fake.bird();
fake.reptile();
fake.fish();
fake.insect();
fake.dog();
fake.cat();

fake.animals().animal();
fake.animals().dog();
```

`animal()` picks from the broad animal pool. Category-specific methods are useful when a fixture needs a particular kind of animal, such as a dog breed or a bird name.

## Helpers

```cpp
const std::vector<std::string> values = {"red", "green", "blue"};
const auto& picked = fake.choice(values);
```

`choice()` returns a random element from a non-empty indexable container and throws `std::invalid_argument` for empty containers.
