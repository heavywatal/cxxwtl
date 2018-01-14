include_directories(
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_compile_options(-DNDEBUG -O3 -march=native -Wall -Wextra -Wpedantic)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  link_libraries(pthread)
endif()

macro(add_test_ filename)
  get_filename_component(name_we ${filename} NAME_WE)
  add_executable(test-${name_we} ${filename})
  add_test(NAME ${name_we} COMMAND $<TARGET_FILE:test-${name_we}>)
endmacro()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} source_files)
foreach(src IN LISTS source_files)
  add_test_(${src})
endforeach()
