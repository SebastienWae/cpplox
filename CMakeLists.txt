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

include(dependencies.cmake)

if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
  set(RE2_BUILD_TESTING OFF)
  add_subdirectory(externals/fuzztest)
  add_subdirectory(tests)
endif()

add_subdirectory(src/lib)
add_subdirectory(src/app)

# if(BUILD_TESTING) set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage
# -fprofile-arcs -ftest-coverage") endif()

# cmake -G 'Ninja Multi-Config' -DCMAKE_CXX_COMPILER=g++-12
# -DCMAKE_TOOLCHAIN_FILE=/workspaces/cpplox/externals/vcpkg/scripts/buildsystems/vcpkg.cmake
# -S/workspaces/cpplox -B/workspaces/cpplox/build cmake --build build --config
# Debug --target cpplox

# ctest --build-and-test /workspaces/cpplox /workspaces/cpplox/build
# --build-generator 'Ninja Multi-Config' --build-noclean --test-command ctest -C
# Debug

# mkdir -p buildDir cd buildDir cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug
# -DBUILD_SHARED_LIBS=ON sourceDir cmake --build . --target clean cmake --build
# . ctest -j 4

# mkdir -p build cd build

# default: -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION -UNDEBUG

# fuzzing flags: -fsanitize-coverage=inline-8bit-counters
# -fsanitize-coverage=trace-cmp -fsanitize=address -DADDRESS_SANITIZER

# enable fuzzing mode: -DFUZZTEST_FUZZING_MODE=on

# enable libfuzzer mode -DFUZZTEST_COMPATIBILITY_MODE=libfuzzer
# -fsanitize=fuzzer-no-link -DFUZZTEST_COMPATIBILITY_MODE "testPresets": [ {
# "name": "gcc", "configurePreset": "gcc", "configuration": "RelWithDebInfo" },
# { "name": "clang", "configurePreset": "clang", "configuration":
# "RelWithDebInfo" }, { "name": "asan", "configurePreset": "clang-sanitize",
# "configuration": "RelWithDebInfo" } ], "packagePresets": [ { "name": "cpplox",
# "configurePreset": "gcc", "generators": [ "ZIP", "TGZ" ], "configurations": [
# "Release" ] }
