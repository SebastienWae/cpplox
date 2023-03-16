#include "Environment.hpp"

#include <fmt/core.h>

#include <optional>
#include <stdexcept>

#include "ErrorReporter.hpp"
#include "Expression.hpp"

Environment::Environment(ErrorReporter &error_reporter)
    : m_error_reporter(error_reporter) {}

Environment::EnvironmentException::EnvironmentException(std::string const &what)
    : std::runtime_error("[InterpreterException]\n" + what) {}

void Environment::define(std::string_view name,
                         std::optional<Interpreter::ExpressionValue> value) {
  m_values.insert_or_assign(name, value);
}

void Environment::assign(Box<AssignExpression> const &expr,
                         Interpreter::ExpressionValue const &value) {
  auto name = expr->getName();
  if (m_values.contains(name)) {
    m_values[name] = value;
  } else {
    throw error(expr->getIdentifier(),
                fmt::format("Undeclared variable '{}'", expr->getName()));
  }
}

auto Environment::get(Box<VariableExpression> const &expr)
    -> std::optional<Interpreter::ExpressionValue> {
  try {
    return m_values.at(expr->getName());
  } catch (std::out_of_range &e) {
    throw error(expr->getIdentifier(),
                fmt::format("Undeclared variable '{}'", expr->getName()));
  }
}

auto Environment::error(std::optional<Token const *> token,
                        std::string const &msg) -> EnvironmentException {
  std::string str;

  if (!token.has_value()) {
    str = fmt::format("{} at end", msg);
  } else {
    auto const *token_value = token.value();
    str = fmt::format("Line: {}\n{}", token_value->getLine(), msg);
  }

  m_error_reporter.setError(ErrorType::RUNTIME_ERROR, str);
  return {str};
}