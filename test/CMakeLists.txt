add_compile_options(-march=native -Wall -Wextra -Wpedantic)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_FLAGS_DEV "-O2 -g")

link_libraries(wtl)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  link_libraries(pthread)
endif()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} source_files)
foreach(src IN LISTS source_files)
  get_filename_component(name_we ${src} NAME_WE)
  add_executable(test-${name_we} ${src})
  add_test(NAME ${name_we} COMMAND $<TARGET_FILE:test-${name_we}>)
endforeach()
