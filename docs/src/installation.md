# Installation

## Vendored Source

Copy `include/faker/faker.hpp` and `src/faker.cpp` into your project. Compile `faker.cpp` with the rest of your application and add the copied `include` directory to your include path.

For one-file tools using the repository layout directly:

```cpp
#define FAKER_CPP_IMPLEMENTATION
#include <faker/faker.hpp>
```

Do not define `FAKER_CPP_IMPLEMENTATION` in a target that also compiles `src/faker.cpp`.

## CMake Subdirectory

Use this when the repository is checked out inside your project:

```cmake
add_subdirectory(external/faker.cpp)
target_link_libraries(my_app PRIVATE faker::faker)
```

## CMake Package

Use this after installing a release package or a local build:

```cmake
find_package(FakerCpp CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE faker::faker)
```

Release archives include the source, headers, CMake package files, README, license, and book source docs. Check the root `LICENSE` file for the Apache-2.0 terms.
