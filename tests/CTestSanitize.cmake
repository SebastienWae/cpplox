# ---------------------------------- config ---------------------------------- #
set(CONFIGURE_OPTIONS "--preset clang")
if(TEST_SANITIZER EQUAL "address")
  set(CTEST_MEMORYCHECK_TYPE=AddressSanitizer PARENT_SCOPE)
  set(CONFIGURE_OPTIONS
      "${CONFIGURE_OPTIONS} -DSANITIZE_ADDRESS=ON"
      PARENT_SCOPE)
elseif()
  set(CTEST_MEMORYCHECK_TYPE=UndefinedBehaviorSanitizer PARENT_SCOPE)
  set(CONFIGURE_OPTIONS
      "${CONFIGURE_OPTIONS} -DSANITIZE_UNDEFINED=ON"
      PARENT_SCOPE)
endif()

if(NOT DEFINED CTEST_MEMORYCHECK_TYPE)
  message(FATAL_ERROR "TEST_SANITIZER must be set to address or undefined")
endif()

set(CTEST_BUILD_NAME
    "${CMAKE_HOST_SYSTEM_NAME}.Sanitize.${CTEST_MEMORY_CHECK_TYPE}")
set(CTEST_CONFIGURATION_TYPE "Debug")

include(${CTEST_SCRIPT_DIRECTORY}/CTestConfig.cmake)

# ----------------------------------- start ---------------------------------- #
ctest_start(Experimental) # TODO: ci

# --------------------------------- configure -------------------------------- #
ctest_configure(OPTIONS ${CONFIGURE_OPTIONS} RETURN_VALUE config_result)
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
