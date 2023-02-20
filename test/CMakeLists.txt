find_package(doctest CONFIG REQUIRED)

add_executable(${PROJECT_NAME}_test "test_main.cpp"
  "LexerTest.cpp")

target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME} doctest::doctest)

add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)

