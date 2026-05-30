cmake_minimum_required(VERSION 3.16)

project(FakerCpp
    VERSION 0.1.3
    DESCRIPTION "A small C++ fake data generation library"
    LANGUAGES CXX)

include(GNUInstallDirs)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(FAKER_TOP_LEVEL_DEFAULT ON)
else()
    set(FAKER_TOP_LEVEL_DEFAULT OFF)
endif()

option(FAKER_BUILD_TESTS "Build Faker.cpp tests" ${FAKER_TOP_LEVEL_DEFAULT})
option(FAKER_BUILD_EXAMPLES "Build Faker.cpp examples" ${FAKER_TOP_LEVEL_DEFAULT})
option(FAKER_BUILD_BENCHMARKS "Build Faker.cpp benchmarks" OFF)

add_library(faker src/faker.cpp)
add_library(faker::faker ALIAS faker)

target_compile_features(faker PUBLIC cxx_std_17)
target_include_directories(faker
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(MSVC)
    target_compile_options(faker PRIVATE /W4 /permissive-)
else()
    target_compile_options(faker PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(FAKER_BUILD_EXAMPLES)
    add_executable(faker_basic examples/basic.cpp)
    target_link_libraries(faker_basic PRIVATE faker::faker)
endif()

if(FAKER_BUILD_TESTS)
    include(CTest)
    add_executable(faker_tests tests/test_faker.cpp)
    target_link_libraries(faker_tests PRIVATE faker::faker)
    add_test(NAME faker_tests COMMAND faker_tests)
endif()

if(FAKER_BUILD_BENCHMARKS)
    add_executable(faker_benchmark bench/benchmark.cpp)
    target_link_libraries(faker_benchmark PRIVATE faker::faker)
endif()

install(TARGETS faker
    EXPORT FakerCppTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES LICENSE CONTRIBUTING.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR})

install(EXPORT FakerCppTargets
    NAMESPACE faker::
    FILE FakerCppTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FakerCpp)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/FakerCppConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FakerCppConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/FakerCppConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FakerCpp)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/FakerCppConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/FakerCppConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FakerCpp)
