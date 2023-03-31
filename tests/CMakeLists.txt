enable_testing()

include(GoogleTest)

add_executable(lexer_test LexerTest.cpp)
target_link_libraries(lexer_test PRIVATE  cpplox::lox)
link_fuzztest(lexer_test)
gtest_discover_tests(lexer_test)