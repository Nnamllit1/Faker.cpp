# Quick Start

```cpp
#include <faker/faker.hpp>

#include <iostream>

int main() {
    faker::Faker fake(42);

    std::cout << fake.full_name() << "\n";
    std::cout << fake.email() << "\n";
    std::cout << fake.street_address() << "\n";
    std::cout << fake.uuid_v4() << "\n";
}
```

Use an explicit `faker::Faker` instance when deterministic output matters. Use namespace-level helpers for quick scripts:

```cpp
faker::seed(123);
auto email = faker::email();
```

