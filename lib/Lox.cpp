#include "Lox.hpp"

#include "Expression.hpp"
#include "Interpreter.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Token.hpp"

auto Lox::run(std::string_view source) -> std::optional<std::string const> {
  m_error_reporter.clearErrors();

  Lexer lexer{source, m_error_reporter};
  auto tokens = lexer.scanTokens();

  if (!hasErrors() && tokens.has_value()) {
    Parser parser{tokens.value(), m_error_reporter};
    auto statements = parser.parse();

    if (!hasErrors() && statements.has_value()) {
      Interpreter interpreter{statements.value(), m_environment,
                              m_error_reporter};
      return interpreter.interpret();
    }
  }

  return std::nullopt;
}

[[nodiscard]] auto Lox::getErrors() const -> std::vector<std::string> {
  auto error = m_error_reporter.getErrors();
  return m_error_reporter.getErrors();
}

[[nodiscard]] auto Lox::hasErrors() const -> bool {
  return m_error_reporter.hasErrors();
}