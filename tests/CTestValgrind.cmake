# ---------------------------------- config ---------------------------------- #
find_program(VALGRIND "valgrind" REQUIRED)

set(CTEST_BUILD_NAME "${CMAKE_HOST_SYSTEM_NAME}.Valgrind")
set(CTEST_CONFIGURATION_TYPE "Debug")

set(CTEST_MEMORYCHECK_COMMAND ${VALGRIND})
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full")

# TODO: add --suppressions=absl.supp

include(${CTEST_SCRIPT_DIRECTORY}/CTestConfig.cmake)

# ----------------------------------- start ---------------------------------- #
ctest_start(Experimental)

# --------------------------------- configure -------------------------------- #
ctest_configure(OPTIONS "--preset gcc" RETURN_VALUE config_result)
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
