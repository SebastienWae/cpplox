# set(CTEST_SCRIPT_DIRECTORY)

include(${CTEST_SCRIPT_DIRECTORY}/CTestConfig.cmake)

site_name(CTEST_SITE)

set(CTEST_BUILD_NAME ${CMAKE_HOST_SYSTEM_NAME})
set(CTEST_SOURCE_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}")
set(CTEST_BINARY_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}/build/testtest")
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
set(CTEST_CONFIGURATION_TYPE Debug)
set(configureOpts
  "-DCMAKE_CXX_COMPILER=g++-12"
  "-DCMAKE_TOOLCHAIN_FILE=/workspaces/cpplox/externals/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CTEST_COVERAGE_COMMAND "gcov-12")

# ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
ctest_start(Experimental)
ctest_configure(OPTIONS "${configureOpts}")
ctest_build()
ctest_test()
ctest_coverage()
ctest_submit()