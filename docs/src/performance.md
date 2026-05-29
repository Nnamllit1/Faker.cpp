# Performance

Faker.cpp includes a benchmark executable for release comparisons and local checks. Release benchmarks are meant to show whether a new release is meaningfully faster or slower than the previous one under similar GitHub-hosted runner conditions.

Build it with:

```sh
cmake -S . -B build -DFAKER_BUILD_TESTS=ON -DFAKER_BUILD_EXAMPLES=ON -DFAKER_BUILD_BENCHMARKS=ON
cmake --build build
```

Run the standard benchmark:

```sh
./build/faker_benchmark --rows 1000000 --runs 10
```

The standard benchmark generates every flat public Faker API once per row. For the current API this is 55 generated values per row, reported as one named result section per API.

The tool writes:

- `performance.json`
- `performance.md`

Release workflows run performance in separate fresh Linux and Windows runner jobs, use 10 runs of 1,000,000 rows, compare matching named result sections against the previous release, and upload one combined `performance.json` plus one combined `performance.md`. Aggregate rows are still included for context, but they are marked as `workload changed` when the generated-result count changes.

Virtual machines and GitHub-hosted runners are useful for release-to-release comparisons, but the numbers should not be treated as absolute hardware limits. Small deltas under 5% are treated as normal runner/runtime variation, not measured slowdowns. When new generators are added, aggregate rows become `workload changed`; use matching per-generator rows to inspect existing generator performance.
