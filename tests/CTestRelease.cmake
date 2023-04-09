# ---------------------------------- config ---------------------------------- #
if(COMPILER_PRESET NOT EQUAL "gcc" OR COMPILER_PRESET NOT EQUAL "clang")
  message(FATAL_ERROR "COMPILER_PRESET must be set to gcc or clang")
endif()

set(CTEST_BUILD_NAME "${CMAKE_HOST_SYSTEM_NAME}.Release.${COMPILER_PRESET}")
set(CTEST_CONFIGURATION_TYPE "RelWithDebInfo")

set(CONFIGURE_OPTIONS "--preset ${COMPILER_PRESET}")
if(COMPILER_PRESET EQUAL "clang" AND ENABLE_COVERAGE)
  set(CONFIGURE_OPTIONS
      "${CONFIGURE_OPTIONS} --coverage"
      PARENT_SCOPE)
endif()

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

# ----------------------------------- test ----------------------------------- #
ctest_test()
ctest_submit(PARTS Test HTTPHEADER ${CDASH_AUTH})

# -------------------------------- coverage -------------------------------- #
ctest_coverage()
ctest_submit(HTTPHEADER "Authorization: Bearer ")

# ---------------------------------- submit ---------------------------------- #
ctest_submit(PARTS Done HTTPHEADER ${CDASH_AUTH})
