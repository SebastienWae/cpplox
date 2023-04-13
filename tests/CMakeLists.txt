enable_testing()

include(GoogleTest)

# ----------------------------------- lexer ---------------------------------- #
add_executable(lexer_test LexerTest.cpp)
target_link_libraries(lexer_test PRIVATE GTest::gtest_main cpplox::lox)
add_sanitizers(lexer_test)
gtest_discover_tests(lexer_test)
