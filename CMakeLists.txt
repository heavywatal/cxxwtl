cmake_minimum_required(VERSION 3.21)
project(wtl
  VERSION 0.11.1
  LANGUAGES CXX)

include(GNUInstallDirs)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

add_library(zlib INTERFACE)
add_library(${PROJECT_NAME}::zlib ALIAS zlib)
find_package(ZLIB)
if(ZLIB_FOUND)
  target_link_libraries(zlib INTERFACE ZLIB::ZLIB)
else()
  target_compile_definitions(zlib INTERFACE "ZLIB_NOT_FOUND")
endif()

add_library(threads INTERFACE)
add_library(${PROJECT_NAME}::threads ALIAS threads)
find_package(Threads)
if(Threads_FOUND)
  target_link_libraries(threads INTERFACE Threads::Threads)
else()
  target_compile_definitions(threads INTERFACE "THREADS_NOT_FOUND")
endif()

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS ${PROJECT_NAME} zlib threads
  EXPORT ${PROJECT_NAME}-targets
)

set(config_destination share/cmake/${PROJECT_NAME})
install(EXPORT ${PROJECT_NAME}-targets
  DESTINATION ${config_destination}
  NAMESPACE ${PROJECT_NAME}::
)

set(version_file ${PROJECT_NAME}-config-version.cmake)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${version_file}
  COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(config.cmake.in ${PROJECT_NAME}-config.cmake
  INSTALL_DESTINATION ${config_destination}
)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${version_file}
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
  DESTINATION ${config_destination}
)

include(CTest)
if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
  add_subdirectory(test)
endif()
