#ifndef CPPLOX_ENVIRONMENT_HPP
#define CPPLOX_ENVIRONMENT_HPP

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Expression.hpp"
#include "Interpreter.hpp"

class Environment {
  std::unordered_map<std::string, std::optional<Interpreter::ExpressionValue>>
      m_values;

  ErrorReporter& m_error_reporter;
  std::optional<std::reference_wrapper<Environment>> m_parent;

 public:
  Environment(ErrorReporter& error_reporter);
  Environment(Environment& parent);

  class EnvironmentException : public std::runtime_error {
   public:
    EnvironmentException(std::string const& what);
  };

  void define(std::string_view name,
              std::optional<Interpreter::ExpressionValue> value);
  void assign(Box<AssignExpression> const& expr,
              Interpreter::ExpressionValue const& value);
  auto get(Box<VariableExpression> const& expr)
      -> std::optional<Interpreter::ExpressionValue>;

 private:
  auto error(std::optional<Token const*> token, std::string const& msg)
      -> EnvironmentException;
};

#endif /* CPPLOX_ENVIRONMENT_HPP */
