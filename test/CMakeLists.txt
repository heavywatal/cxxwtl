cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_FLAGS "-DNDEBUG -O3 -march=native -Wall -Wextra -Wpedantic")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(test-concurrent concurrent.cpp)
add_test(NAME concurrent COMMAND $<TARGET_FILE:test-concurrent>)

add_executable(test-genetic genetic.cpp)
add_test(NAME genetic COMMAND $<TARGET_FILE:test-genetic>)

add_executable(test-iostr iostr.cpp)
add_test(NAME iostr COMMAND $<TARGET_FILE:test-iostr>)

add_executable(test-numeric numeric.cpp)
add_test(NAME numeric COMMAND $<TARGET_FILE:test-numeric>)

add_executable(test-random random.cpp)
add_test(NAME random COMMAND $<TARGET_FILE:test-random>)
