link_libraries(${PROJECT_NAME})

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  add_compile_options(-stdlib=libc++)
endif()
add_compile_options(-march=native -Wall -Wextra -pedantic)
set(CMAKE_CXX_FLAGS_DEV "-O2 -g")

set(source_files
  cluster.cpp
  dataframe.cpp
  filesystem.cpp
  genetic.cpp
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

find_package(Threads)
if(Threads_FOUND)
  set(source_files ${source_files} chrono.cpp)
  set(source_files ${source_files} concurrent.cpp)
  link_libraries(Threads::Threads)
endif()

find_package(ZLIB)
if(ZLIB_FOUND)
  set(source_files ${source_files} zlib.cpp)
  link_libraries(ZLIB::ZLIB)
endif()

foreach(src IN LISTS source_files)
  get_filename_component(name_we ${src} NAME_WE)
  add_executable(test-${name_we} ${src})
  set_target_properties(test-${name_we} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
  )
  add_test(NAME ${name_we} COMMAND $<TARGET_FILE:test-${name_we}>)
endforeach()
