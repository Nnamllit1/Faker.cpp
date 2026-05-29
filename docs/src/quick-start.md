# Quick Start

Create a `faker::Faker` instance and call the generators you need:

```cpp
#include <faker/faker.hpp>

#include <iostream>

int main() {
    faker::Faker fake(42);

    std::cout << fake.full_name() << "\n";
    std::cout << fake.email() << "\n";
    std::cout << fake.street_address() << "\n";
    std::cout << fake.uuid_v4() << "\n";
    std::cout << fake.crypto().sha256() << "\n";
}
```

Pass a seed when deterministic output matters:

```cpp
faker::Faker fake(123);

auto first = fake.email();
auto second = fake.email();
```

Use namespace-level helpers for quick scripts:

```cpp
faker::seed(123);
auto email = faker::email();
```

The library supports both flat methods and lightweight category aliases:

```cpp
faker::Faker fake(42);

auto flat = fake.email();
auto categorized = fake.internet().email();
```
