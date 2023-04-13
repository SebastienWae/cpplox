set(CTEST_PROJECT_NAME "cpplox")
set(CTEST_NIGHTLY_START_TIME "01:00:00 UTC")

set(CTEST_DROP_METHOD "https")
set(CTEST_DROP_SITE "my.cdash.org")
set(CTEST_DROP_LOCATION "/submit.php?project=cpplox")
set(CTEST_DROP_SITE_CDASH TRUE)

if(ENV{CI})
  site_name("github")
else()
  site_name(CTEST_SITE)
endif()

set(CTEST_SOURCE_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}/../../")
set(CTEST_BINARY_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}/../../build")
set(CTEST_CMAKE_GENERATOR "Ninja Multi-Config")

set(CTEST_USE_LAUNCHERS_DEFAULT YES)

if(NOT DEFINED ENV{CDASH_TOKEN})
  message(FATAL_ERROR "CDash auth token is missing")
endif()

set(CDASH_AUTH "Authorization: Bearer $ENV{CDASH_TOKEN}")
