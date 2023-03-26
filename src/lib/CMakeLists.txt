add_library(lox
  Lox.hpp
  Lox.cpp
  ErrorReporter.hpp
  ErrorReporter.cpp
  SourcePosition.hpp
  SourcePosition.cpp
  Token.hpp
  Lexer.hpp
  Lexer.cpp
  Expression.hpp
  Expression.cpp
  Statement.hpp
  Statement.cpp
  Parser.hpp
  Parser.cpp
  Interpreter.hpp
  Interpreter.cpp
  Environment.hpp
  Environment.cpp
  utils/Enums.hpp
  utils/Box.hpp)

target_link_libraries(lox
  PRIVATE
  fmt::fmt
  range-v3::meta
  range-v3::concepts
  range-v3::range-v3)

add_library(cpplox::lox ALIAS lox)

# set_target_properties(cpplox
#   PROPERTIES
#   ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# if (SANITIZE_ADDRESS OR SANITIZE_MEMORY OR SANITIZE_THREAD OR SANITIZE_UNDEFINED)
#   add_sanitizers(${PROJECT_NAME})
#   target_compile_options(${PROJECT_NAME} PRIVATE -fno-sanitize-recover=all)
# endif()
