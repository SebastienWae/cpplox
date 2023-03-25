#include "Environment.hpp"

Environment::Environment(ErrorReporter &error_reporter)
    : m_error_reporter(error_reporter), m_parent(std::nullopt) {}

Environment::Environment(Environment &parent)
    : m_error_reporter(parent.m_error_reporter), m_parent(parent) {}

Environment::EnvironmentException::EnvironmentException(std::string const &what)
    : std::runtime_error("[InterpreterException]\n" + what) {}

void Environment::define(std::string_view name,
                         std::optional<Interpreter::ExpressionValue> value) {
  m_values.insert_or_assign(std::string{name}, value);
}

void Environment::assign(Box<AssignExpression> const &expr,
                         Interpreter::ExpressionValue const &value) {
  auto const name = std::string{expr->getName()};
  if (m_values.contains(name)) {
    m_values[name] = value;
  } else if (m_parent.has_value()) {
    m_parent.value().get().assign(expr, value);
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
    if (m_parent.has_value()) {
      return m_parent.value().get().get(expr);
    }
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