find_package(fmt CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)

set(HEADER_LIST
  "Lox.hpp"
  "ErrorReporter.hpp"
  "Token.hpp"
  "Lexer.hpp"
  "Expression.hpp"
  "Statement.hpp"
  "Parser.hpp"
  "Interpreter.hpp"
  "Environment.hpp"
  "utils/Enums.hpp"
  "utils/Box.hpp")

set(SOURCE_LIST
  "Lox.cpp"
  "ErrorReporter.cpp"
  "Token.cpp"
  "Lexer.cpp"
  "Expression.cpp"
  "Statement.cpp"
  "Parser.cpp"
  "Interpreter.cpp"
  "Environment.cpp")

add_library(${PROJECT_NAME} ${HEADER_LIST} ${SOURCE_LIST})

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  fmt::fmt
  range-v3::meta
  range-v3::concepts
  range-v3::range-v3)

set_target_properties(${PROJECT_NAME}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if (SANITIZE_ADDRESS OR SANITIZE_MEMORY OR SANITIZE_THREAD OR SANITIZE_UNDEFINED)
  add_sanitizers(${PROJECT_NAME})
  target_compile_options(${PROJECT_NAME} PRIVATE -fno-sanitize-recover=all)
endif()
