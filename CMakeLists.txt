cmake_minimum_required(VERSION 3.14)
project(Ditto)

option(BUILD_DITTO_TESTS "Builds tests for the Ditto library" OFF)
option(USE_STD_TEMPLATES "Uses containers from the STL instead of versions from Ditto if available" OFF)
option(DITTO_TARGET_ARCH "Configures the target architecture" x86_64)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

set(DITTO_SOURCES
        src/assert.cpp
        src/hash.cpp
        src/crc32c.cpp
        )

if (DITTO_TARGET_ARCH MATCHES x86_64)
    set(DITTO_SOURCES
            ${DITTO_SOURCES}
            src/arch/x86_64.cpp
            )
elseif (DITTO_TARGET_ARCH MATCHES aarch64)
    set(DITTO_SOURCES
            ${DITTO_SOURCES}
            src/arch/aarch64.cpp
            )
endif ()

add_library(
        Ditto
        STATIC
        ${DITTO_SOURCES}
)
target_include_directories(Ditto PUBLIC include)

target_compile_features(Ditto PUBLIC cxx_std_20)

target_compile_options(Ditto PUBLIC -fno-omit-frame-pointer)

if (BUILD_DITTO_TESTS)

    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    FetchContent_MakeAvailable(googletest)

    # We use the address sanitizer to ensure we are not leaking memory or triggering undefined behavior
    set(DITTO_BUILD_OPTIONS
            -fsanitize=address,undefined
            -g3
            -Os
            )

    if (USE_STD_TEMPLATES)
        set(DITTO_BUILD_OPTIONS
                ${DITTO_BUILD_OPTIONS}
                -DUSE_STD_TEMPLATES=
                )
    endif ()

    enable_testing()

    add_executable(
            DittoTests
            test/linked_list.cpp
            test/pair.cpp
            test/hash_map.cpp
            test/linear_map.cpp
            test/box.cpp
            test/static_ptr.cpp
            test/state_machine.cpp
            test/circular_queue.cpp
            test/non_null_ptr.cpp
            test/assert.cpp
            test/event_loop.cpp
            test/result.cpp
            test/badge.cpp
            test/fixed.cpp
            test/resource_lock.cpp
            test/optional.cpp
            test/span.cpp
            test/binary_search_tree.cpp
            test/red_black_tree.cpp
            test/enumerate.cpp
            test/fixed_flat_map.cpp
            test/fixed_vector.cpp
            test/enum.cpp
    )

    target_include_directories(DittoTests PRIVATE test)

    target_link_libraries(
            DittoTests
            Ditto
            gtest_main
            gmock
    )

    target_compile_features(DittoTests PRIVATE cxx_std_20)

    target_compile_options(
            gtest_main
            PRIVATE
            ${DITTO_BUILD_OPTIONS}
    )

    target_compile_options(
            gtest
            PRIVATE
            ${DITTO_BUILD_OPTIONS}
    )

    target_compile_options(
            gmock
            PRIVATE
            ${DITTO_BUILD_OPTIONS}
    )

    target_compile_options(
            DittoTests
            PRIVATE
            ${DITTO_BUILD_OPTIONS}
    )

    target_link_options(
            DittoTests
            PRIVATE
            ${DITTO_BUILD_OPTIONS}
    )

    include(GoogleTest)
    gtest_discover_tests(DittoTests)

endif ()
