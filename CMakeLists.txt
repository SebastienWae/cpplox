cmake_minimum_required(VERSION 3.22)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

find_package(Sanitizers)

project(cpplox 
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_CLANG_TIDY clang-tidy-15)
set(CMAKE_CXX_CPPCHECK cppcheck)

set(sources
  "src/main.cpp")
set(headers)

add_executable(${PROJECT_NAME} ${headers} ${sources})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)