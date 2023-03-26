set(CTEST_USE_LAUNCHERS_DEFAULT YES)

include(${CTEST_SCRIPT_DIRECTORY}/CTestConfig.cmake)

site_name(CTEST_SITE)

set(CTEST_BUILD_NAME ${CMAKE_HOST_SYSTEM_NAME})

set(CTEST_SOURCE_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}")
set(CTEST_BINARY_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}/ctest")

set(CTEST_CMAKE_GENERATOR "Ninja Multi-Config")
set(CTEST_CONFIGURATION_TYPE Debug)

# set(CTEST_COVERAGE_COMMAND "gcov-12")

ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})

ctest_start(Experimental GROUP Debug)

set(cfgOptions
  "-DCMAKE_CXX_COMPILER=g++-12"
  "-DCMAKE_TOOLCHAIN_FILE=/workspaces/cpplox/externals/vcpkg/scripts/buildsystems/vcpkg.cmake")
ctest_configure(OPTIONS "${cfgOptions}")
ctest_submit(
  PARTS Start Configure
  HTTPHEADER "Authorization: Bearer ")

ctest_build()
ctest_submit(
  PARTS Build
  HTTPHEADER "Authorization: Bearer ")

ctest_test()
ctest_submit(
  PARTS Test
  HTTPHEADER "Authorization: Bearer ")

# ctest_coverage()
# ctest_submit(HTTPHEADER "Authorization: Bearer ")
