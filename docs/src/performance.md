# Performance

Faker.cpp includes a benchmark executable for release comparisons and local checks.

Build it with:

```sh
cmake -S . -B build -DFAKER_BUILD_TESTS=ON -DFAKER_BUILD_EXAMPLES=ON -DFAKER_BUILD_BENCHMARKS=ON
cmake --build build
```

Run the standard benchmark:

```sh
./build/faker_benchmark --rows 1000000 --runs 10
```

The standard benchmark generates every flat public Faker API once per row. For the current API this is 48 generated values per row, reported as one named result section per API.

The tool writes:

- `performance.json`
- `performance.md`

Release workflows run performance in separate fresh Linux and Windows runner jobs, use 10 runs of 1,000,000 rows, compare matching named result sections against the previous release, and upload one combined `performance.json` plus one combined `performance.md`. Aggregate rows are still included for context, but they are marked as a changed workload when the generated-result count changes.

Virtual machines and GitHub-hosted runners are useful for release-to-release comparisons, but the numbers should not be treated as absolute hardware limits.
