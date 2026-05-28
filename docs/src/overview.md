# Overview

Faker.cpp generates fake but realistic-looking data for tests, prototypes, demos, and local development tools.

The design goal is close to Dear ImGui integration style: a normal C++ header and source file that can be vendored directly, with CMake support for projects that want package-style consumption.

The first release focuses on common data:

- names and usernames
- email, domains, URLs, and IPv4 addresses
- phone numbers and addresses
- companies and job titles
- lorem-like words, sentences, and paragraphs
- dates, numbers, booleans, UUIDs, and selection helpers

