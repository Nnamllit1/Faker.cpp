# Platform Support

Faker.cpp targets C++17 and uses only the standard library.

Tested in CI:

- Linux with GCC
- Windows with MSVC

The `v0.1.0` release treats Linux and Windows as the supported tested platforms. macOS is expected to work because the implementation avoids platform-specific dependencies, but it is not tested in CI yet.

The library does not require networking, locale databases, generated code, or third-party runtime dependencies.
