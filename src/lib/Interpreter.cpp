#include "Interpreter.hpp"

#include "Environment.hpp"

Interpreter::Interpreter(std::vector<Statement> const& statements,
                         Environment& environment,
                         ErrorReporter& error_reporter)
    : m_statements(statements),
      m_environment(environment),
      m_error_reporter(error_reporter) {}

auto Interpreter::interpret() -> std::optional<std::vector<std::string> const> {
  try {
    StatementVisitor v{*this, m_environment};
    for (auto statement : m_statements) {
      std::visit(v, statement);
    }
    return v.getValues();
  } catch (Interpreter::InterpreterException& e) {
  } catch (Environment::EnvironmentException& e) {
  } catch (...) {
    m_error_reporter.setError("Unexpected error while interpreting");
  }
  return std::nullopt;
}

auto Interpreter::error(std::optional<Token const*> token,
                        std::string const& msg) -> InterpreterException {
  if (token.has_value()) {
    m_error_reporter.setError(msg, token.value()->getPosition());
  } else {
    m_error_reporter.setError(msg);
  }
  return {msg};
}

Interpreter::InterpreterException::InterpreterException(std::string const& what)
    : std::runtime_error("[InterpreterException]\n" + what) {}

auto Interpreter::isTruthy(ExpressionValue const& value) -> bool {
  TruthyVisitor v;
  return std::visit(v, value);
}

auto Interpreter::isEqual(ExpressionValue const& left_value,
                          ExpressionValue const& right_value) -> bool {
  EqualityVisitor v;
  return std::visit(v, left_value, right_value);
}

Interpreter::StatementVisitor::StatementVisitor(Interpreter& interpreter,
                                                Environment& env)
    : m_interpreter(interpreter), m_env(env) {}

[[nodiscard]] auto Interpreter::StatementVisitor::getValues() const
    -> std::vector<std::string> const& {
  return m_values;
}

auto Interpreter::StatementVisitor::operator()(Box<PrintStatement> const& s)
    -> void {
  ExpressionVisitor expression_visitor{m_interpreter, m_env};
  ExpressionValue value = std::visit(expression_visitor, s->getExpression());
  StringifyVisitor stringify_visitor;
  m_values.emplace_back(std::visit(stringify_visitor, value));
}
auto Interpreter::StatementVisitor::operator()(
    Box<ExpressionStatement> const& s) -> void {
  ExpressionVisitor expression_visitor{m_interpreter, m_env};
  std::visit(expression_visitor, s->getExpression());
}

auto Interpreter::StatementVisitor::operator()(
    Box<VariableDeclaration> const& s) -> void {
  auto initializer = s->getInitializer();
  std::optional<ExpressionValue> value = std::nullopt;
  if (initializer.has_value()) {
    ExpressionVisitor expression_visitor{m_interpreter, m_env};
    value = std::visit(expression_visitor, initializer.value());
  }
  m_env.define(s->getName(), value);
}

auto Interpreter::StatementVisitor::operator()(Box<BlockStatement> const& s)
    -> void {
  Environment env{m_interpreter.m_environment};
  Interpreter::StatementVisitor visitor{m_interpreter, env};
  for (auto statement : s->getStatements()) {
    std::visit(visitor, statement);
  }
  for (auto const& v : visitor.getValues()) {
    m_values.emplace_back(v);
  }
}

Interpreter::ExpressionVisitor::ExpressionVisitor(Interpreter& interpreter,
                                                  Environment& env)
    : m_interpreter(interpreter), m_env(env) {}

auto Interpreter::ExpressionVisitor::operator()(
    Box<LiteralNumberExpression> const& e) -> ExpressionValue {
  return e->getValue();
}
auto Interpreter::ExpressionVisitor::operator()(
    [[maybe_unused]] Box<LiteralStringExpression> const& e) -> ExpressionValue {
  return std::string{e->getValue()};
}
auto Interpreter::ExpressionVisitor::operator()(
    [[maybe_unused]] Box<LiteralExpression<TokenType::TOKEN_TRUE>> const& e)
    -> ExpressionValue {
  return true;
}
auto Interpreter::ExpressionVisitor::operator()(
    [[maybe_unused]] Box<LiteralExpression<TokenType::TOKEN_FALSE>> const& e)
    -> ExpressionValue {
  return false;
}
auto Interpreter::ExpressionVisitor::operator()(
    [[maybe_unused]] Box<LiteralExpression<TokenType::TOKEN_NIL>> const& e)
    -> ExpressionValue {
  return nullptr;
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<VariableExpression> const& e) -> ExpressionValue {
  auto value = m_env.get(e);

  if (value.has_value()) {
    return value.value();
  }

  throw m_interpreter.error(
      e->getIdentifier(), fmt::format("Undefined variable '{}'", e->getName()));
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<GroupingExpression> const& e) -> ExpressionValue {
  return std::visit(*this, e->getExpression());
}
auto Interpreter::ExpressionVisitor::operator()(Box<TernaryExpression> const& e)
    -> ExpressionValue {
  auto condition_value = std::visit(*this, e->getConditionExpression());

  if (isTruthy(condition_value)) {
    return std::visit(*this, e->getTrueExpression());
  }
  return std::visit(*this, e->getFalseExpression());
}
auto Interpreter::ExpressionVisitor::operator()(Box<AssignExpression> const& e)
    -> ExpressionValue {
  auto value = std::visit(*this, e->getValue());
  m_env.assign(e, value);
  return value;
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<UnaryExpression<TokenType::TOKEN_MINUS>> const& e) -> ExpressionValue {
  auto right_value = std::visit(*this, e->getExpression());

  if (auto const* pval = std::get_if<double>(&right_value)) {
    return -(*pval);
  }

  throw m_interpreter.error(e->getOperator(), "Operand must be a number");
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<UnaryExpression<TokenType::TOKEN_BANG>> const& e) -> ExpressionValue {
  auto right_value = std::visit(*this, e->getExpression());

  return !isTruthy(right_value);
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<BinaryExpression<TokenType::TOKEN_EQUAL_EQUAL>> const& e)
    -> ExpressionValue {
  auto left_value = std::visit(*this, e->getLeftExpression());
  auto right_value = std::visit(*this, e->getRightExpression());

  return isEqual(left_value, right_value);
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<BinaryExpression<TokenType::TOKEN_BANG_EQUAL>> const& e)
    -> ExpressionValue {
  auto left_value = std::visit(*this, e->getLeftExpression());
  auto right_value = std::visit(*this, e->getRightExpression());

  return !isEqual(left_value, right_value);
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<BinaryExpression<TokenType::TOKEN_LESS>> const& e) -> ExpressionValue {
  auto left_value = std::visit(*this, e->getLeftExpression());
  auto right_value = std::visit(*this, e->getRightExpression());

  auto const* pl_val = std::get_if<double>(&left_value);
  auto const* pr_val = std::get_if<double>(&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr)) {
    return *pl_val < *pr_val;
  }

  throw m_interpreter.error(e->getOperator(), "Operands must be numbers");
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<BinaryExpression<TokenType::TOKEN_LESS_EQUAL>> const& e)
    -> ExpressionValue {
  auto left_value = std::visit(*this, e->getLeftExpression());
  auto right_value = std::visit(*this, e->getRightExpression());

  auto const* pl_val = std::get_if<double>(&left_value);
  auto const* pr_val = std::get_if<double>(&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr)) {
    return *pl_val <= *pr_val;
  }

  throw m_interpreter.error(e->getOperator(), "Operands must be numbers");
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<BinaryExpression<TokenType::TOKEN_GREATER>> const& e)
    -> ExpressionValue {
  auto left_value = std::visit(*this, e->getLeftExpression());
  auto right_value = std::visit(*this, e->getRightExpression());

  auto const* pl_val = std::get_if<double>(&left_value);
  auto const* pr_val = std::get_if<double>(&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr)) {
    return *pl_val > *pr_val;
  }

  throw m_interpreter.error(e->getOperator(), "Operands must be numbers");
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<BinaryExpression<TokenType::TOKEN_GREATER_EQUAL>> const& e)
    -> ExpressionValue {
  auto left_value = std::visit(*this, e->getLeftExpression());
  auto right_value = std::visit(*this, e->getRightExpression());

  auto const* pl_val = std::get_if<double>(&left_value);
  auto const* pr_val = std::get_if<double>(&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr)) {
    return *pl_val >= *pr_val;
  }

  throw m_interpreter.error(e->getOperator(), "Operands must be numbers");
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<BinaryExpression<TokenType::TOKEN_PLUS>> const& e) -> ExpressionValue {
  auto left_value = std::visit(*this, e->getLeftExpression());
  auto right_value = std::visit(*this, e->getRightExpression());

  auto const* left_double_ptr = std::get_if<double>(&left_value);
  auto const* right_double_ptr = std::get_if<double>(&right_value);
  if ((left_double_ptr != nullptr) && (right_double_ptr != nullptr)) {
    return *left_double_ptr + *right_double_ptr;
  }

  auto const* left_string_ptr = std::get_if<std::string>(&left_value);
  auto const* right_string_ptr = std::get_if<std::string>(&right_value);
  if ((left_string_ptr != nullptr) && (right_string_ptr != nullptr)) {
    return *left_string_ptr + *right_string_ptr;
  }

  throw m_interpreter.error(e->getOperator(),
                            "Operands must be two numbers or two strings");
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<BinaryExpression<TokenType::TOKEN_MINUS>> const& e) -> ExpressionValue {
  auto left_value = std::visit(*this, e->getLeftExpression());
  auto right_value = std::visit(*this, e->getRightExpression());

  auto const* pl_val = std::get_if<double>(&left_value);
  auto const* pr_val = std::get_if<double>(&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr)) {
    return *pl_val - *pr_val;
  }

  throw m_interpreter.error(e->getOperator(), "Operands must be numbers");
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<BinaryExpression<TokenType::TOKEN_STAR>> const& e) -> ExpressionValue {
  auto left_value = std::visit(*this, e->getLeftExpression());
  auto right_value = std::visit(*this, e->getRightExpression());

  auto const* pl_val = std::get_if<double>(&left_value);
  auto const* pr_val = std::get_if<double>(&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr)) {
    return *pl_val * *pr_val;
  }

  throw m_interpreter.error(e->getOperator(), "Operands must be numbers");
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<BinaryExpression<TokenType::TOKEN_SLASH>> const& e) -> ExpressionValue {
  auto left_value = std::visit(*this, e->getLeftExpression());
  auto right_value = std::visit(*this, e->getRightExpression());

  auto const* pl_val = std::get_if<double>(&left_value);
  auto const* pr_val = std::get_if<double>(&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr)) {
    return *pl_val / *pr_val;
  }

  throw m_interpreter.error(e->getOperator(), "Operands must be numbers");
}
auto Interpreter::ExpressionVisitor::operator()(
    Box<BinaryExpression<TokenType::TOKEN_COMMA>> const& e) -> ExpressionValue {
  std::visit(*this, e->getLeftExpression());
  return std::visit(*this, e->getRightExpression());
}

auto Interpreter::TruthyVisitor::operator()(bool const& v) -> bool { return v; }
auto Interpreter::TruthyVisitor::operator()([[maybe_unused]] double const& v)
    -> bool {
  return true;
}
auto Interpreter::TruthyVisitor::operator()(
    [[maybe_unused]] std::string const& v) -> bool {
  return true;
}
auto Interpreter::TruthyVisitor::operator()(
    [[maybe_unused]] std::nullptr_t const& v) -> bool {
  return false;
}

auto Interpreter::EqualityVisitor::operator()(bool const& left,
                                              bool const& right) -> bool {
  return left == right;
}
auto Interpreter::EqualityVisitor::operator()(
    [[maybe_unused]] bool const& left, [[maybe_unused]] double const& right)
    -> bool {
  return false;
}
auto Interpreter::EqualityVisitor::operator()(
    [[maybe_unused]] bool const& left,
    [[maybe_unused]] std::string const& right) -> bool {
  return false;
}
auto Interpreter::EqualityVisitor::operator()(
    [[maybe_unused]] bool const& left,
    [[maybe_unused]] std::nullptr_t const& right) -> bool {
  return false;
}
auto Interpreter::EqualityVisitor::operator()(
    [[maybe_unused]] double const& left, [[maybe_unused]] bool const& right)
    -> bool {
  return false;
}
auto Interpreter::EqualityVisitor::operator()(double const& left,
                                              double const& right) -> bool {
  return left == right;
}
auto Interpreter::EqualityVisitor::operator()(
    [[maybe_unused]] double const& left,
    [[maybe_unused]] std::string const& right) -> bool {
  return false;
}
auto Interpreter::EqualityVisitor::operator()(
    [[maybe_unused]] double const& left,
    [[maybe_unused]] std::nullptr_t const& right) -> bool {
  return false;
}
auto Interpreter::EqualityVisitor::operator()(
    [[maybe_unused]] std::string const& left,
    [[maybe_unused]] bool const& right) -> bool {
  return false;
}
auto Interpreter::EqualityVisitor::operator()(
    [[maybe_unused]] std::string const& left,
    [[maybe_unused]] double const& right) -> bool {
  return false;
}
auto Interpreter::EqualityVisitor::operator()(std::string const& left,
                                              std::string const& right)
    -> bool {
  return left == right;
}
auto Interpreter::EqualityVisitor::operator()(
    [[maybe_unused]] std::string const& left,
    [[maybe_unused]] std::nullptr_t const& right) -> bool {
  return false;
}
auto Interpreter::EqualityVisitor::operator()(
    [[maybe_unused]] std::nullptr_t const& left,
    [[maybe_unused]] bool const& right) -> bool {
  return false;
}
auto Interpreter::EqualityVisitor::operator()(
    [[maybe_unused]] std::nullptr_t const& left,
    [[maybe_unused]] double const& right) -> bool {
  return false;
}
auto Interpreter::EqualityVisitor::operator()(
    [[maybe_unused]] std::nullptr_t const& left,
    [[maybe_unused]] std::string const& right) -> bool {
  return false;
}
auto Interpreter::EqualityVisitor::operator()(
    [[maybe_unused]] std::nullptr_t const& left,
    [[maybe_unused]] std::nullptr_t const& right) -> bool {
  return true;
}

auto Interpreter::StringifyVisitor::operator()(bool const& v) -> std::string {
  return v ? "TRUE" : "FALSE";
}
auto Interpreter::StringifyVisitor::operator()(double const& v) -> std::string {
  return std::to_string(v);
}
auto Interpreter::StringifyVisitor::operator()(std::string const& v)
    -> std::string {
  return v;
}
auto Interpreter::StringifyVisitor::operator()(
    [[maybe_unused]] std::nullptr_t const& v) -> std::string {
  return "NIL";
}