cmake_minimum_required(VERSION 3.22)

project(cpplox 
  VERSION 0.0.1
  LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  configure_file(
    "${PROJECT_SOURCE_DIR}/include/config.hpp.in"
    "${PROJECT_BINARY_DIR}/include/config.hpp"
  )

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
  find_package(Sanitizers)

  include(CTest)
endif()

add_subdirectory(src)
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
  add_subdirectory(externals/doctest)
  add_subdirectory(test)
endif()
