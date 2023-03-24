cmake_minimum_required(VERSION 3.22)

project(cpplox 
  VERSION 0.0.1
  LANGUAGES CXX)


include(CTest)

# if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  configure_file(
    "${PROJECT_SOURCE_DIR}/include/config.hpp.in"
    "${PROJECT_SOURCE_DIR}/include/config.hpp"
  )

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
  find_package(Sanitizers)
# endif()

if(BUILD_TESTING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
endif()

add_subdirectory(lib)
add_subdirectory(app)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
