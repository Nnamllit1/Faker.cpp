# Quick Start

Create a `faker::Faker` instance and call the generators you need:

```cpp
#include <faker/faker.hpp>

#include <iostream>

int main() {
    faker::Faker fake(42);

    std::cout << fake.full_name() << "\n";
    std::cout << fake.email() << "\n";
    std::cout << fake.full_address() << "\n";
    std::cout << fake.animals().animal() << "\n";
    std::cout << fake.uuid_v4() << "\n";
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
auto pet = faker::dog();
```

The library supports both flat methods and lightweight category aliases:

```cpp
faker::Faker fake(42);

auto flat = fake.email();
auto categorized = fake.internet().email();
auto animal = fake.animals().mammal();
```

Build richer test records by composing independent generators:

```cpp
faker::Faker fake(42);

auto name = fake.full_name();
auto contact = fake.email();
auto address = fake.mailing_address();
auto favorite_animal = fake.animal();
```
