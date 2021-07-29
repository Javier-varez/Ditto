cmake_minimum_required(VERSION 3.14)
project(containers)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_library(
  ate_containers
  STATIC
  src/assert.cpp
)
target_include_directories(ate_containers PUBLIC inc)

target_compile_features(ate_containers PUBLIC cxx_std_20)

option(BUILD_CONTAINER_TESTS "Builds the containers tests" OFF)

if(BUILD_CONTAINER_TESTS)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

set(ATE_BUILD_OPTIONS
  -fsanitize=address
  -g3
  -Os
)

enable_testing()

add_executable(
  containers_test
  test/linked_list.cpp
  test/pair.cpp
  test/hash_map.cpp
  test/linear_map.cpp
  test/box.cpp
  test/polymorphic_object.cpp
  test/state_machine.cpp
  test/circular_queue.cpp
  test/non_null_ptr.cpp
  test/assert.cpp
  test/event_loop.cpp
)

target_include_directories(ate_containers PRIVATE test)

target_link_libraries(
  containers_test
  ate_containers
  gtest_main
  gmock
)

target_compile_features(containers_test INTERFACE cxx_std_20)

target_compile_options(
  gtest_main
  PRIVATE
  ${ATE_BUILD_OPTIONS}
)

target_compile_options(
  gtest
  PRIVATE
  ${ATE_BUILD_OPTIONS}
)

target_compile_options(
  gmock
  PRIVATE
  ${ATE_BUILD_OPTIONS}
)

target_compile_options(
  containers_test
  PRIVATE
  ${ATE_BUILD_OPTIONS}
)

target_link_options(
  containers_test
  PRIVATE
  ${ATE_BUILD_OPTIONS}
)

include(GoogleTest)
gtest_discover_tests(containers_test)

endif()
