enable_testing()

add_executable(lexer_test "test_main.cpp" "LexerTest.cpp")
target_link_libraries(lexer_test PRIVATE doctest::doctest cpplox::lox)
add_test(NAME lexer COMMAND lexer_test)

# add_executable(${PROJECT_NAME}_ParserTeskkt "test_main.cpp" "ParserTest.cpp")
# target_link_libraries(${PROJECT_NAME}_ParserTest PRIVATE ${PROJECT_NAME} doctest::doctest)
# add_test(NAME ParserTest COMMAND ${PROJECT_NAME}_ParserTest)

# add_executable(${PROJECT_NAME}_InterpreterTest "test_main.cpp" "InterpreterTest.cpp")
# target_link_libraries(${PROJECT_NAME}_InterpreterTest PRIVATE ${PROJECT_NAME} doctest::doctest)
# add_test(NAME InterpreterTest COMMAND ${PROJECT_NAME}_InterpreterTest)