cmake_minimum_required(VERSION 3.14)
project(containers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  containers_test
  test/linked_list.cpp
  test/pair.cpp
)
target_link_libraries(
  containers_test
  gtest_main
  gmock
)
target_include_directories(
  containers_test
  PRIVATE
  inc
)

include(GoogleTest)
gtest_discover_tests(containers_test)
