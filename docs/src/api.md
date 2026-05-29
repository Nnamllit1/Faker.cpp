# API

## Generator

`faker::Faker` owns its random engine. Construct it with a seed for deterministic output:

```cpp
faker::Faker fake(1234);
```

## Categories

Person:

```cpp
fake.first_name();
fake.last_name();
fake.full_name();
fake.name_prefix();
fake.name_suffix();
fake.middle_name();
```

Internet:

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
```

Location and business:

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
fake.company_name();
fake.job_title();
```

Text and primitives:

```cpp
fake.word();
fake.sentence();
fake.paragraph();
fake.boolean();
fake.number_int(1, 10);
fake.number_real(0.0, 1.0);
fake.uuid_v4();
```

Dates return `YYYY-MM-DD` strings:

```cpp
fake.past_date();
fake.future_date();
fake.recent_date();
```

Finance and commerce:

```cpp
fake.credit_card_number();
fake.currency_code();
fake.iban();
fake.product_name();
fake.product_category();
fake.price();
```

Crypto-shaped test data:

```cpp
fake.hex_string();
fake.md5();
fake.sha1();
fake.sha256();
```

These values are random-looking fake data for tests. They are not cryptographic primitives.

System and colors:

```cpp
fake.file_name();
fake.file_path();
fake.mime_type();
fake.semver();
fake.hex_color();
fake.rgb_color();
```

Category aliases call the same generator:

```cpp
fake.person().full_name();
fake.person().name_prefix();
fake.internet().email();
fake.location().street_address();
fake.company().name();
fake.lorem().sentence();
fake.finance().credit_card_number();
fake.commerce().product_name();
fake.crypto().sha256();
fake.system().file_name();
fake.color().hex_color();
```
