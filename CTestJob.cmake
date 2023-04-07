set(CTEST_USE_LAUNCHERS_DEFAULT YES)

include(${CTEST_SCRIPT_DIRECTORY}/CTestConfig.cmake)

site_name(CTEST_SITE)

set(CTEST_BUILD_NAME ${CMAKE_HOST_SYSTEM_NAME})

set(CTEST_SOURCE_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}")
set(CTEST_BINARY_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}/build")

set(CTEST_CMAKE_GENERATOR "Ninja Multi-Config")

# set(CTEST_MEMORYCHECK_TYPE AddressSanitizer UndefinedBehaviorSanitizer)
# set(CTEST_MEMORYCHECK_TYPE AddressSanitizer)
set(CTEST_MEMORYCHECK_COMMAND /usr/bin/valgrind)
# set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full
# --show-leak-kinds=all")

set(CDASH_AUTH "Authorization: Bearer $ENV{CDASH_TOKEN}")

ctest_start(Experimental)

ctest_configure(OPTIONS "--preset clang")
ctest_submit(PARTS Start Configure HTTPHEADER ${CDASH_AUTH})

ctest_build()
ctest_submit(PARTS Build HTTPHEADER ${CDASH_AUTH})

ctest_memcheck()
ctest_submit(PARTS MemCheck)

# ctest_test() ctest_submit(PARTS Test HTTPHEADER ${CDASH_AUTH})

ctest_submit(PARTS Done HTTPHEADER ${CDASH_AUTH})

# ctest_coverage() ctest_submit(HTTPHEADER "Authorization: Bearer ")

# ctest --script CTestJob.cmake --progress  --output-on-failure --build-config
# RelWithDebInfo

# sanitizer valgrind covergage

# set(CTEST_COVERAGE_COMMAND "gcov-12") --covergage
