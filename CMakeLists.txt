cmake_minimum_required(VERSION 3.25)

project(
  cpplox
  VERSION 0.0.1
  LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
  include(cmake/UseCompilerCache.cmake)
  usecompilercache()

  include(CTest)

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

configure_file("${PROJECT_SOURCE_DIR}/include/config.hpp.in"
               "${PROJECT_SOURCE_DIR}/include/config.hpp")

include(cmake/Dependencies.cmake)

add_subdirectory(src/lib)
add_subdirectory(src/app)

if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
  add_subdirectory(tests)
endif()
