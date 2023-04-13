# ----------------------------------- ctest ---------------------------------- #
set(CTEST_BUILD_NAME "${CMAKE_HOST_SYSTEM_NAME}.Sanitize.${TEST_SANITIZER}")

set(CTEST_CONFIGURATION_TYPE "Debug")

set(CONFIGURE_OPTIONS "--preset clang")
if(TEST_SANITIZER STREQUAL "address")
  set(CTEST_MEMORYCHECK_TYPE AddressSanitizer)
  set(CONFIGURE_OPTIONS ${CONFIGURE_OPTIONS} "-DSANITIZE_ADDRESS=ON")
elseif(TEST_SANITIZER STREQUAL "undefined")
  set(CTEST_MEMORYCHECK_TYPE UndefinedBehaviorSanitizer)
  set(CONFIGURE_OPTIONS ${CONFIGURE_OPTIONS} "-DSANITIZE_UNDEFINED=ON")
endif()

if(NOT DEFINED CTEST_MEMORYCHECK_TYPE)
  message(FATAL_ERROR "TEST_SANITIZER must be set to address or undefined")
endif()

include(${CTEST_SCRIPT_DIRECTORY}/CTestConfig.cmake)

# ----------------------------------- start ---------------------------------- #
if(ENV{CI})
  ctest_start(Continuous)
else()
  ctest_start(Experimental)
endif()

# --------------------------------- configure -------------------------------- #
ctest_configure(OPTIONS "${CONFIGURE_OPTIONS}" RETURN_VALUE config_result)
ctest_submit(PARTS Start Configure HTTPHEADER ${CDASH_AUTH})

if(config_result)
  ctest_submit(PARTS Done HTTPHEADER ${CDASH_AUTH})
  message(FATAL_ERROR "Configuration step failed")
endif()

# ----------------------------------- build ---------------------------------- #
ctest_build(RETURN_VALUE build_result)
ctest_submit(PARTS Build HTTPHEADER ${CDASH_AUTH})

if(build_result)
  ctest_submit(PARTS Done HTTPHEADER ${CDASH_AUTH})
  message(FATAL_ERROR "Build step failed")
endif()

# --------------------------------- memcheck --------------------------------- #
ctest_memcheck()
ctest_submit(PARTS MemCheck)

# ---------------------------------- submit ---------------------------------- #
ctest_submit(PARTS Done HTTPHEADER ${CDASH_AUTH})
