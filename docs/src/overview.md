# Overview

Faker.cpp generates fake but realistic-looking data for tests, prototypes, demos, and local development tools.

The design goal is close to Dear ImGui integration style: a normal C++ header and source file that can be vendored directly, with CMake support for projects that want package-style consumption.

The `v0.1.0` release is the first stable baseline for the current API and package layout. A future `1.0` release should only make stronger compatibility promises after the API, documentation, release assets, and tested platform story have had more real use.

The current release focuses on common data:

- names and usernames
- email, domains, URLs, IPv4, IPv6, and MAC addresses
- phone numbers and addresses
- animals, pets, and broad animal taxonomy labels
- companies and job titles
- lorem-like words, sentences, and paragraphs
- dates, numbers, booleans, UUIDs, and selection helpers
- commerce, finance, colors, file names, semantic versions, and fake hash-shaped strings
