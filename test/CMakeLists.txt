find_package(doctest CONFIG REQUIRED)

add_executable(${PROJECT_NAME}_LexerTest "test_main.cpp" "LexerTest.cpp")
target_link_libraries(${PROJECT_NAME}_LexerTest PRIVATE ${PROJECT_NAME} doctest::doctest)
add_test(NAME LexerTest COMMAND ${PROJECT_NAME}_LexerTest)

# add_executable(${PROJECT_NAME}_ParserTest "test_main.cpp" "ParserTest.cpp")
# target_link_libraries(${PROJECT_NAME}_ParserTest PRIVATE ${PROJECT_NAME} doctest::doctest)
# add_test(NAME ParserTest COMMAND ${PROJECT_NAME}_ParserTest)

# add_executable(${PROJECT_NAME}_InterpreterTest "test_main.cpp" "InterpreterTest.cpp")
# target_link_libraries(${PROJECT_NAME}_InterpreterTest PRIVATE ${PROJECT_NAME} doctest::doctest)
# add_test(NAME InterpreterTest COMMAND ${PROJECT_NAME}_InterpreterTest)