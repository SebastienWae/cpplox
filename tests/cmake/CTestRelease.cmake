# ----------------------------------- ctest ---------------------------------- #
if(NOT (COMPILER_PRESET STREQUAL "gcc" OR COMPILER_PRESET STREQUAL "clang"))
  message(FATAL_ERROR "COMPILER_PRESET must be set to gcc or clang")
endif()

set(CTEST_BUILD_NAME "${CMAKE_HOST_SYSTEM_NAME}.Release.${COMPILER_PRESET}")

set(CTEST_CONFIGURATION_TYPE "RelWithDebInfo")

set(CONFIGURE_OPTIONS "--preset ${COMPILER_PRESET}")
# if(COMPILER_PRESET STREQUAL "clang" AND CODE_COVERAGE) set(CONFIGURE_OPTIONS
# ${CONFIGURE_OPTIONS} "-DCMAKE_CXX_FLAGS=--coverage")

# find_program(LLVM_COV "llvm-cov-15" REQUIRED) set(CTEST_COVERAGE_COMMAND
# LLVM_COV) set(CTEST_COVERAGE_EXTRA_FLAGS gcov) endif()

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

# ----------------------------------- test ----------------------------------- #
ctest_test()
ctest_submit(PARTS Test HTTPHEADER ${CDASH_AUTH})

# -------------------------------- coverage -------------------------------- #
# if(COMPILER_PRESET STREQUAL "clang" AND CODE_COVERAGE) ctest_coverage()
# ctest_submit(PARTS Coverage HTTPHEADER ${CDASH_AUTH}) endif()

# ---------------------------------- submit ---------------------------------- #
ctest_submit(PARTS Done HTTPHEADER ${CDASH_AUTH})
