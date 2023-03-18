#include "Lox.hpp"

#include <range/v3/view/join.hpp>
#include <utility>

#include "Expression.hpp"
#include "Interpreter.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Token.hpp"

Lox::Lox() : m_environment(m_error_reporter) {}

Lox::Lox(ErrorReporter::Formater error_formater)
    : m_error_reporter(std::move(error_formater)),
      m_environment(m_error_reporter) {}

auto Lox::run(std::string_view source)
    -> std::optional<std::vector<std::string> const> {
  m_error_reporter.clearErrors();

  m_error_reporter.setSource(source);

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