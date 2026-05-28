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
```

Internet:

```cpp
fake.username();
fake.email();
fake.domain_name();
fake.url();
fake.ipv4();
```

Location and business:

```cpp
fake.phone_number();
fake.street_address();
fake.city();
fake.country();
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

