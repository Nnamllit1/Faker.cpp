# Faker.cpp

Faker.cpp is a small C++17 fake data generation library inspired by faker-style libraries in JavaScript. The `v0.1.0` release is the first stable project baseline: Linux and Windows are tested in CI, while macOS should be portable in theory but is not part of the tested matrix yet.

The library is intentionally simple: include the header, compile the source file, and generate data from a local `faker::Faker` instance.

```cpp
#include <faker/faker.hpp>

#include <iostream>

int main() {
    faker::Faker fake(42);

    std::cout << fake.full_name() << '\n';
    std::cout << fake.email() << '\n';
    std::cout << fake.full_address() << '\n';
    std::cout << fake.animals().animal() << '\n';
    std::cout << fake.uuid_v4() << '\n';
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

## Release Status

`v0.1.0` is intended as the first stable baseline for the current API, packaging, and release workflow. A future `1.0` release should mean the public API, package layout, platform support, and documentation are ready to be treated as long-term compatibility promises.

## Performance Benchmarks

Build the benchmark target with:

```sh
cmake -S . -B build -DFAKER_BUILD_TESTS=ON -DFAKER_BUILD_EXAMPLES=ON -DFAKER_BUILD_BENCHMARKS=ON
cmake --build build
```

Run the full "everything" benchmark:

```sh
./build/faker_benchmark --rows 1000000 --runs 2
```

The benchmark generates every flat public Faker API once per row, reports one named result section per API, writes `performance.json` and `performance.md`, and prints throughput to stdout. Release builds run performance in five separate fresh Linux runner jobs and five separate fresh Windows runner jobs. Each runner uses 2 runs of 1,000,000 rows, and the release report combines those runner instances into one median per OS before comparing matching named result sections against the previous release. Aggregate rows are included for context and marked as a changed workload when the generated-result count changes.

Benchmark numbers from virtual machines and GitHub-hosted runners are useful for comparing one release to another under similar conditions. Small deltas under 5% are treated as normal runner/runtime variation, not measured slowdowns. Aggregate rows are marked as `workload changed` when new generators change the generated-result count, so per-generator rows are the best place to inspect existing generator performance.

## API Snapshot

`faker::Faker` supports:

- person: `first_name`, `last_name`, `full_name`, `name_prefix`, `name_suffix`, `middle_name`
- internet: `username`, `email`, `domain_name`, `url`, `ipv4`, `ipv6`, `mac_address`, `password`, `user_agent`
- location: `building_number`, `street_name`, `street_address`, `secondary_address`, `full_address`, `mailing_address`, `city`, `country`, `state`, `zip_code`, `postal_code`, `latitude`, `longitude`
- business: `company_name`, `job_title`
- text: `word`, `sentence`, `paragraph`
- finance: `credit_card_number`, `currency_code`, `iban`
- commerce: `product_name`, `product_category`, `price`
- crypto-shaped test data: `hex_string`, `md5`, `sha1`, `sha256`
- system/dev: `file_name`, `file_path`, `mime_type`, `semver`
- colors: `hex_color`, `rgb_color`
- animals: `animal`, `animal_type`, `mammal`, `bird`, `reptile`, `fish`, `insect`, `dog`, `cat`
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
auto pet = fake.animals().dog();
```

The hash helpers are fake random-looking strings for tests and fixtures. They are not cryptographic primitives.

## License

Faker.cpp is licensed under the Apache License 2.0.

See [LICENSE](LICENSE) for the full terms. The project may choose a different license in the future, but this release uses Apache-2.0.
