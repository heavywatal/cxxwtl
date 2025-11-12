add_compile_definitions("_LIBCPP_REMOVE_TRANSITIVE_INCLUDES")
add_compile_options(
  -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow
  $<IF:$<BOOL:$<CONFIG>>,,-Werror>
  $<IF:$<BOOL:$<CONFIG>>,,-O2>
  $<IF:$<BOOL:$<CONFIG>>,,-g>
  $<$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},x86_64>:-march=native>
  $<$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},arm64>:-march=armv8.4-a>
)

set(source_files
  cluster.cpp
  dataframe.cpp
  filesystem.cpp
  iostr.cpp
  math.cpp
  numeric.cpp
  random.cpp
  scope.cpp
  signed.cpp
  string.cpp
)

if(UNIX)
  set(source_files ${source_files} resource.cpp)
endif()

function(add_executable_test src)
  cmake_path(GET src STEM name_we)
  add_executable(test-${name_we} ${src})
  set_target_properties(test-${name_we} PROPERTIES CXX_EXTENSIONS OFF)
  target_link_libraries(test-${name_we} PRIVATE ${PROJECT_NAME})
  add_test(NAME ${name_we} COMMAND $<TARGET_FILE:test-${name_we}>)
endfunction()

foreach(src IN LISTS source_files)
  add_executable_test(${src})
endforeach()

target_compile_options(test-random PRIVATE -Wno-float-equal)

if(ZLIB_FOUND)
  add_executable_test(zlib.cpp)
  target_link_libraries(test-zlib PRIVATE wtl::zlib)
endif()

if(Threads_FOUND)
  add_executable_test(chrono.cpp)
  target_link_libraries(test-chrono PRIVATE wtl::threads)
  add_executable_test(concurrent.cpp)
  target_link_libraries(test-concurrent PRIVATE wtl::threads)
endif()
