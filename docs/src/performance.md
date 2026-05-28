# Performance

Faker.cpp includes a benchmark executable for release comparisons and local checks.

Build it with:

```sh
cmake -S . -B build -DFAKER_BUILD_TESTS=ON -DFAKER_BUILD_EXAMPLES=ON -DFAKER_BUILD_BENCHMARKS=ON
cmake --build build
```

Run the standard benchmark:

```sh
./build/faker_benchmark --rows 1000000
```

The standard benchmark generates every flat public Faker API once per row. For the current API this is 48 generated values per row.

The tool writes:

- `performance.json`
- `performance.md`

Release workflows upload per-platform reports directly as release assets. If a previous release has a matching performance JSON asset, the new report includes faster/slower percentages for runtime and throughput.

Virtual machines and GitHub-hosted runners are useful for release-to-release comparisons, but the numbers should not be treated as absolute hardware limits.
