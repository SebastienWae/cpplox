set(CTEST_SOURCE_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}/../")
set(CTEST_BINARY_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}/../build")

set(CTEST_CMAKE_GENERATOR "Ninja Multi-Config")

# address sanitizer
# memory sanitizer
# undefined behavior sanitizer
# valgrind
# coverage

ctest_start()

ctest_configure(OPTIONS "${CONFIGURE_OPTIONS}" RETURN_VALUE config_result)
if(config_result)
  message(FATAL_ERROR "Configuration step failed")
endif()

ctest_build(RETURN_VALUE build_result)
if(build_result)
  message(FATAL_ERROR "Build step failed")
endif()

ctest_test()
