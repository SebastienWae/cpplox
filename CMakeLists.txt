cmake_minimum_required(VERSION 3.22)

project(cpplox 
  VERSION 0.0.1
  LANGUAGES CXX)

find_program(CCACHE_EXECUTABLE ccache)
if(CCACHE_EXECUTABLE)
  set(ccacheEnv CCACHE_SLOPPINESS=pch_defines,time_macros)
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CMAKE_COMMAND} -E env ${ccacheEnv} ${CCACHE_EXECUTABLE})
endif()

include(CTest)
# set(CTEST_USE_LAUNCHERS OFF)

# configure_file(
#   "${PROJECT_SOURCE_DIR}/include/config.hpp.in"
#   "${PROJECT_SOURCE_DIR}/include/config.hpp"
# )

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# include(dependencies.cmake)

add_subdirectory(src/lib)
# add_subdirectory(src/app)

# if(BUILD_TESTING AND (TEST_LOX OR CMAKE_SOURCE_DIR STREQUAL cpplox_SOURCE_DIR))
#   add_subdirectory(tests)
# endif()

# if(BUILD_TESTING)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
# endif()