include_directories(
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_compile_options(-DNDEBUG -O3 -march=native -Wall -Wextra -Wpedantic)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  link_libraries(pthread)
endif()

macro(add_test_named name)
  add_executable(test-${name} ${name}.cpp)
  add_test(NAME ${name} COMMAND $<TARGET_FILE:test-${name}>)
endmacro()

add_test_named(concurrent)
add_test_named(genetic)
add_test_named(iostr)
add_test_named(numeric)
add_test_named(random)
