cmake_minimum_required(VERSION 3.22)

project(cpplox 
  VERSION 0.0.1
  LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
  include(CTest)

  find_program(CCACHE_EXECUTABLE ccache)
  if(CCACHE_EXECUTABLE)
    set(ccacheEnv CCACHE_SLOPPINESS=pch_defines,time_macros)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CMAKE_COMMAND} -E env ${ccacheEnv} ${CCACHE_EXECUTABLE})
  endif()

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

configure_file(
  "${PROJECT_SOURCE_DIR}/include/config.hpp.in"
  "${PROJECT_SOURCE_DIR}/include/config.hpp"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(dependencies.cmake)

add_subdirectory(src/lib)
add_subdirectory(src/app)

if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
  add_subdirectory(tests)
endif()

# if(BUILD_TESTING)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
# endif()

# cmake -G 'Ninja Multi-Config' -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_TOOLCHAIN_FILE=/workspaces/cpplox/externals/vcpkg/scripts/buildsystems/vcpkg.cmake -S/workspaces/cpplox -B/workspaces/cpplox/build
# cmake --build build --config Debug --target cpplox


# ctest --build-and-test /workspaces/cpplox /workspaces/cpplox/build --build-generator 'Ninja Multi-Config' --build-noclean --test-command ctest -C Debug


# mkdir -p buildDir
# cd buildDir
# cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON sourceDir cmake --build . --target clean
# cmake --build .
# ctest -j 4

# mkdir -p build
# cd build
