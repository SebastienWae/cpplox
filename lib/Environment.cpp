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
  m_values.insert_or_assign(std::string{name}, value);
}

void Environment::assign(Box<AssignExpression> const &expr,
                         Interpreter::ExpressionValue const &value) {
  auto const name = std::string{expr->getName().data()};
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
    return m_values.at(std::string{expr->getName()});
  } catch (std::out_of_range &e) {
    throw error(expr->getIdentifier(),
                fmt::format("Undeclared variable '{}'", expr->getName()));
  }
}

auto Environment::error(std::optional<Token const *> token,
                        std::string const &msg) -> EnvironmentException {
  if (token.has_value()) {
    m_error_reporter.setError(msg, token.value()->getPosition());
  } else {
    m_error_reporter.setError(msg);
  }
  return {msg};
}