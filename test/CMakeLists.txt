cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_FLAGS "-DNDEBUG -O3 -march=native -Wall -Wextra -Wpedantic")
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(test-all test.cpp)
add_test(NAME all
  COMMAND $<TARGET_FILE:test-all>
)
