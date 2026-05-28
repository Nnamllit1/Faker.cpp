# Faker.cpp

Faker.cpp is a small C++17 fake data generation library inspired by faker-style libraries in JavaScript. It targets Linux and Windows first. macOS should be portable in theory, but it is not part of the tested CI matrix yet.

The library is intentionally simple: include the header, compile the source file, and generate data from a local `faker::Faker` instance.

```cpp
#include <faker/faker.hpp>

#include <iostream>

int main() {
    faker::Faker fake(42);

    std::cout << fake.full_name() << '\n';
    std::cout << fake.email() << '\n';
    std::cout << fake.uuid_v4() << '\n';
    std::cout << fake.crypto().sha256() << '\n';
}
```

## Use With CMake

```cmake
add_subdirectory(path/to/faker.cpp)
target_link_libraries(your_app PRIVATE faker::faker)
```

Or install it and use:

```cmake
find_package(FakerCpp CONFIG REQUIRED)
target_link_libraries(your_app PRIVATE faker::faker)
```

## ImGui-Style Vendoring

Copy these two files into your project:

- `include/faker/faker.hpp`
- `src/faker.cpp`

Then compile `src/faker.cpp` with your application and make `include/` available on the include path.

For small tools that keep the repository layout intact, you can also build one translation unit:

```cpp
#define FAKER_CPP_IMPLEMENTATION
#include <faker/faker.hpp>
```

Use either this macro or compile `src/faker.cpp`, not both.

## Build

```sh
cmake -S . -B build -DFAKER_BUILD_TESTS=ON -DFAKER_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Performance Benchmarks

Build the benchmark target with:

```sh
cmake -S . -B build -DFAKER_BUILD_TESTS=ON -DFAKER_BUILD_EXAMPLES=ON -DFAKER_BUILD_BENCHMARKS=ON
cmake --build build
```

Run the full "everything" benchmark:

```sh
./build/faker_benchmark --rows 1000000 --runs 10
```

The benchmark generates every flat public Faker API once per row, writes `performance.json` and `performance.md`, and prints throughput to stdout. Release builds run performance in separate fresh Linux and Windows runner jobs, use 10 runs of 1,000,000 rows, compare the median result against the previous release, and attach one combined `performance.json` plus one combined `performance.md`.

Benchmark numbers from virtual machines and GitHub-hosted runners are useful for comparing one release to another under similar conditions. They are not absolute hardware claims.

## API Snapshot

`faker::Faker` supports:

- person: `first_name`, `last_name`, `full_name`
- internet: `username`, `email`, `domain_name`, `url`, `ipv4`, `ipv6`, `mac_address`, `password`, `user_agent`
- location: `street_address`, `city`, `country`, `state`, `zip_code`, `latitude`, `longitude`
- business: `company_name`, `job_title`
- text: `word`, `sentence`, `paragraph`
- finance: `credit_card_number`, `currency_code`, `iban`
- commerce: `product_name`, `product_category`, `price`
- crypto-shaped test data: `hex_string`, `md5`, `sha1`, `sha256`
- system/dev: `file_name`, `file_path`, `mime_type`, `semver`
- colors: `hex_color`, `rgb_color`
- primitives: `boolean`, `number_int`, `number_real`, `uuid_v4`
- dates: `past_date`, `future_date`, `recent_date`, `date_between`
- helpers: `choice`

There are also namespace-level convenience functions such as `faker::email()` and `faker::uuid_v4()` backed by a thread-local default generator.

Category aliases are available when that reads better:

```cpp
faker::Faker fake(42);

auto name = fake.person().full_name();
auto email = fake.internet().email();
auto hash = fake.crypto().sha256();
auto price = fake.commerce().price();
```

The hash helpers are fake random-looking strings for tests and fixtures. They are not cryptographic primitives.

## License

No license chosen yet.
