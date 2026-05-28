# Installation

## Vendored Source

Copy `include/faker/faker.hpp` and `src/faker.cpp` into your project. Compile `faker.cpp` with the rest of your application and add the `include` directory to your include path.

For one-file tools using the repository layout directly:

```cpp
#define FAKER_CPP_IMPLEMENTATION
#include <faker/faker.hpp>
```

Do not define `FAKER_CPP_IMPLEMENTATION` in a target that also compiles `src/faker.cpp`.

## CMake Subdirectory

```cmake
add_subdirectory(external/faker.cpp)
target_link_libraries(my_app PRIVATE faker::faker)
```

## CMake Package

After installation:

```cmake
find_package(FakerCpp CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE faker::faker)
```
