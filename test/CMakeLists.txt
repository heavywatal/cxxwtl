if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Dev)
endif()
set(CMAKE_CXX_FLAGS_DEV "-O2 -g")

set(source_files
  cluster.cpp
  dataframe.cpp
  filesystem.cpp
  iostr.cpp
  math.cpp
  numeric.cpp
  random.cpp
  scope.cpp
  string.cpp
)

if(UNIX)
  set(source_files ${source_files} resource.cpp)
endif()

function(add_executable_test src)
  get_filename_component(name_we ${src} NAME_WE)
  add_executable(test-${name_we} ${src})
  set_target_properties(test-${name_we} PROPERTIES CXX_EXTENSIONS OFF)
  target_link_libraries(test-${name_we} PRIVATE ${PROJECT_NAME})
  target_compile_options(test-${name_we} PRIVATE -Wall -Wextra -pedantic)
  add_test(NAME ${name_we} COMMAND $<TARGET_FILE:test-${name_we}>)
endfunction()

foreach(src IN LISTS source_files)
  add_executable_test(${src})
endforeach()

add_executable_test(zlib.cpp)
target_link_libraries(test-zlib PRIVATE zlib)

find_package(Threads)
if(Threads_FOUND)
  add_executable_test(chrono.cpp)
  target_link_libraries(test-chrono PRIVATE Threads::Threads)
  add_executable_test(concurrent.cpp)
  target_link_libraries(test-concurrent PRIVATE Threads::Threads)
endif()
