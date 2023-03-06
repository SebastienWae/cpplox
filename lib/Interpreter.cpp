#include "Interpreter.hpp"
#include <string>
#include <variant>

Interpreter::Interpreter (Expression const &expression,
                          ErrorReporter &error_reporter)
    : m_expression (expression), m_error_reporter (error_reporter),
      m_visitor (*this)
{
}

auto
Interpreter::interpret () -> std::optional<std::string const>
{
  try
    {
      auto value = std::visit (m_visitor, m_expression);
      StringifyVisitor v;
      return std::visit (v, value);
    }
  catch (Interpreter::InterpreterException &e)
    {
      m_error_reporter.setError ("Interpreter", "Unexpected error");
      return std::nullopt;
    }
}

auto
Interpreter::error (std::optional<Token const *> token, std::string const &msg)
    -> InterpreterException
{
  std::string str;
  if (!token.has_value ())
    {
      str = fmt::format ("{} at end", msg);
    }
  else
    {
      auto const *token_value = token.value ();
      str = fmt::format ("Line: {}\n{}", token_value->getLine (), msg);
    }
  m_error_reporter.setError ("Interpreter", str);
  return { str };
}

Interpreter::InterpreterException::InterpreterException (
    std::string const &what)
    : std::runtime_error ("[InterpreterException]\n" + what)
{
}

auto
Interpreter::isTruthy (ValueType const &value) -> bool
{
  TruthyVisitor v;
  return std::visit (v, value);
}

auto
Interpreter::isEqual (ValueType const &left_value,
                      ValueType const &right_value) -> bool
{
  EqualityVisitor v;
  return std::visit (v, left_value, right_value);
}

auto
Interpreter::Visitor::operator() (Box<LiteralNumberExpression> const &e)
    -> ValueType
{
  return e->getValue ();
}
auto
Interpreter::Visitor::operator() (
    [[maybe_unused]] Box<LiteralStringExpression> const &e) -> ValueType
{
  return std::string{ e->getValue () };
}
auto
Interpreter::Visitor::operator() (
    [[maybe_unused]] Box<LiteralExpression<TokenType::TOKEN_TRUE> > const &e)
    -> ValueType
{
  return true;
}
auto
Interpreter::Visitor::operator() (
    [[maybe_unused]] Box<LiteralExpression<TokenType::TOKEN_FALSE> > const &e)
    -> ValueType
{
  return false;
}
auto
Interpreter::Visitor::operator() (
    [[maybe_unused]] Box<LiteralExpression<TokenType::TOKEN_NIL> > const &e)
    -> ValueType
{
  return nullptr;
}
auto
Interpreter::Visitor::operator() (Box<GroupingExpression> const &e)
    -> ValueType
{
  return std::visit (*this, e->getExpression ());
}
auto
Interpreter::Visitor::operator() (Box<TernaryExpression> const &e) -> ValueType
{
  auto condition_value = std::visit (*this, e->getConditionExpression ());

  if (isTruthy (condition_value))
    {
      return std::visit (*this, e->getTrueExpression ());
    }
  return std::visit (*this, e->getFalseExpression ());
}
auto
Interpreter::Visitor::operator() (
    Box<UnaryExpression<TokenType::TOKEN_MINUS> > const &e) -> ValueType
{
  auto right_value = std::visit (*this, e->getExpression ());

  if (auto const *pval = std::get_if<double> (&right_value))
    {
      return -(*pval);
    }

  throw m_interpreter.error (e->getOperator (), "Operand must be a number");
}
auto
Interpreter::Visitor::operator() (
    Box<UnaryExpression<TokenType::TOKEN_BANG> > const &e) -> ValueType
{
  auto right_value = std::visit (*this, e->getExpression ());

  return !isTruthy (right_value);
}
auto
Interpreter::Visitor::operator() (
    Box<BinaryExpression<TokenType::TOKEN_EQUAL_EQUAL> > const &e) -> ValueType
{
  auto left_value = std::visit (*this, e->getLeftExpression ());
  auto right_value = std::visit (*this, e->getRightExpression ());

  return isEqual (left_value, right_value);
}
auto
Interpreter::Visitor::operator() (
    Box<BinaryExpression<TokenType::TOKEN_BANG_EQUAL> > const &e) -> ValueType
{
  auto left_value = std::visit (*this, e->getLeftExpression ());
  auto right_value = std::visit (*this, e->getRightExpression ());

  return !isEqual (left_value, right_value);
}
auto
Interpreter::Visitor::operator() (
    Box<BinaryExpression<TokenType::TOKEN_LESS> > const &e) -> ValueType
{
  auto left_value = std::visit (*this, e->getLeftExpression ());
  auto right_value = std::visit (*this, e->getRightExpression ());

  auto const *pl_val = std::get_if<double> (&left_value);
  auto const *pr_val = std::get_if<double> (&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr))
    {
      return *pl_val < *pr_val;
    }

  throw m_interpreter.error (e->getOperator (), "Operands must be numbers");
}
auto
Interpreter::Visitor::operator() (
    Box<BinaryExpression<TokenType::TOKEN_LESS_EQUAL> > const &e) -> ValueType
{
  auto left_value = std::visit (*this, e->getLeftExpression ());
  auto right_value = std::visit (*this, e->getRightExpression ());

  auto const *pl_val = std::get_if<double> (&left_value);
  auto const *pr_val = std::get_if<double> (&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr))
    {
      return *pl_val <= *pr_val;
    }

  throw m_interpreter.error (e->getOperator (), "Operands must be numbers");
}
auto
Interpreter::Visitor::operator() (
    Box<BinaryExpression<TokenType::TOKEN_GREATER> > const &e) -> ValueType
{
  auto left_value = std::visit (*this, e->getLeftExpression ());
  auto right_value = std::visit (*this, e->getRightExpression ());

  auto const *pl_val = std::get_if<double> (&left_value);
  auto const *pr_val = std::get_if<double> (&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr))
    {
      return *pl_val > *pr_val;
    }

  throw m_interpreter.error (e->getOperator (), "Operands must be numbers");
}
auto
Interpreter::Visitor::operator() (
    Box<BinaryExpression<TokenType::TOKEN_GREATER_EQUAL> > const &e)
    -> ValueType
{
  auto left_value = std::visit (*this, e->getLeftExpression ());
  auto right_value = std::visit (*this, e->getRightExpression ());

  auto const *pl_val = std::get_if<double> (&left_value);
  auto const *pr_val = std::get_if<double> (&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr))
    {
      return *pl_val >= *pr_val;
    }

  throw m_interpreter.error (e->getOperator (), "Operands must be numbers");
}
auto
Interpreter::Visitor::operator() (
    Box<BinaryExpression<TokenType::TOKEN_PLUS> > const &e) -> ValueType
{
  auto left_value = std::visit (*this, e->getLeftExpression ());
  auto right_value = std::visit (*this, e->getRightExpression ());

  auto const *left_double_ptr = std::get_if<double> (&left_value);
  auto const *right_double_ptr = std::get_if<double> (&right_value);
  if ((left_double_ptr != nullptr) && (right_double_ptr != nullptr))
    {
      return *left_double_ptr + *right_double_ptr;
    }

  auto const *left_string_ptr = std::get_if<std::string> (&left_value);
  auto const *right_string_ptr = std::get_if<std::string> (&right_value);
  if ((left_string_ptr != nullptr) && (right_string_ptr != nullptr))
    {
      return *left_string_ptr + *right_string_ptr;
    }

  throw m_interpreter.error (e->getOperator (),
                             "Operands must be two numbers or two strings");
}
auto
Interpreter::Visitor::operator() (
    Box<BinaryExpression<TokenType::TOKEN_MINUS> > const &e) -> ValueType
{
  auto left_value = std::visit (*this, e->getLeftExpression ());
  auto right_value = std::visit (*this, e->getRightExpression ());

  auto const *pl_val = std::get_if<double> (&left_value);
  auto const *pr_val = std::get_if<double> (&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr))
    {
      return *pl_val - *pr_val;
    }

  throw m_interpreter.error (e->getOperator (), "Operands must be numbers");
}
auto
Interpreter::Visitor::operator() (
    Box<BinaryExpression<TokenType::TOKEN_STAR> > const &e) -> ValueType
{
  auto left_value = std::visit (*this, e->getLeftExpression ());
  auto right_value = std::visit (*this, e->getRightExpression ());

  auto const *pl_val = std::get_if<double> (&left_value);
  auto const *pr_val = std::get_if<double> (&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr))
    {
      return *pl_val * *pr_val;
    }

  throw m_interpreter.error (e->getOperator (), "Operands must be numbers");
}
auto
Interpreter::Visitor::operator() (
    Box<BinaryExpression<TokenType::TOKEN_SLASH> > const &e) -> ValueType
{
  auto left_value = std::visit (*this, e->getLeftExpression ());
  auto right_value = std::visit (*this, e->getRightExpression ());

  auto const *pl_val = std::get_if<double> (&left_value);
  auto const *pr_val = std::get_if<double> (&right_value);
  if ((pl_val != nullptr) && (pr_val != nullptr))
    {
      return *pl_val / *pr_val;
    }

  throw m_interpreter.error (e->getOperator (), "Operands must be numbers");
}
auto
Interpreter::Visitor::operator() (
    Box<BinaryExpression<TokenType::TOKEN_COMMA> > const &e) -> ValueType
{
  std::visit (*this, e->getLeftExpression ());
  return std::visit (*this, e->getRightExpression ());
}

auto
Interpreter::TruthyVisitor::operator() (bool const &v) -> bool
{
  return v;
}
auto
Interpreter::TruthyVisitor::operator() ([[maybe_unused]] double const &v)
    -> bool
{
  return true;
}
auto
Interpreter::TruthyVisitor::operator() ([[maybe_unused]] std::string const &v)
    -> bool
{
  return true;
}
auto
Interpreter::TruthyVisitor::operator() (
    [[maybe_unused]] std::nullptr_t const &v) -> bool
{
  return false;
}

auto
Interpreter::EqualityVisitor::operator() (bool const &left, bool const &right)
    -> bool
{
  return left == right;
}
auto
Interpreter::EqualityVisitor::operator() ([[maybe_unused]] bool const &left,
                                          [[maybe_unused]] double const &right)
    -> bool
{
  return false;
}
auto
Interpreter::EqualityVisitor::operator() (
    [[maybe_unused]] bool const &left,
    [[maybe_unused]] std::string const &right) -> bool
{
  return false;
}
auto
Interpreter::EqualityVisitor::operator() (
    [[maybe_unused]] bool const &left,
    [[maybe_unused]] std::nullptr_t const &right) -> bool
{
  return false;
}
auto
Interpreter::EqualityVisitor::operator() ([[maybe_unused]] double const &left,
                                          [[maybe_unused]] bool const &right)
    -> bool
{
  return false;
}
auto
Interpreter::EqualityVisitor::operator() (double const &left,
                                          double const &right) -> bool
{
  return left == right;
}
auto
Interpreter::EqualityVisitor::operator() (
    [[maybe_unused]] double const &left,
    [[maybe_unused]] std::string const &right) -> bool
{
  return false;
}
auto
Interpreter::EqualityVisitor::operator() (
    [[maybe_unused]] double const &left,
    [[maybe_unused]] std::nullptr_t const &right) -> bool
{
  return false;
}
auto
Interpreter::EqualityVisitor::operator() (
    [[maybe_unused]] std::string const &left,
    [[maybe_unused]] bool const &right) -> bool
{
  return false;
}
auto
Interpreter::EqualityVisitor::operator() (
    [[maybe_unused]] std::string const &left,
    [[maybe_unused]] double const &right) -> bool
{
  return false;
}
auto
Interpreter::EqualityVisitor::operator() (std::string const &left,
                                          std::string const &right) -> bool
{
  return left == right;
}
auto
Interpreter::EqualityVisitor::operator() (
    [[maybe_unused]] std::string const &left,
    [[maybe_unused]] std::nullptr_t const &right) -> bool
{
  return false;
}
auto
Interpreter::EqualityVisitor::operator() (
    [[maybe_unused]] std::nullptr_t const &left,
    [[maybe_unused]] bool const &right) -> bool
{
  return false;
}
auto
Interpreter::EqualityVisitor::operator() (
    [[maybe_unused]] std::nullptr_t const &left,
    [[maybe_unused]] double const &right) -> bool
{
  return false;
}
auto
Interpreter::EqualityVisitor::operator() (
    [[maybe_unused]] std::nullptr_t const &left,
    [[maybe_unused]] std::string const &right) -> bool
{
  return false;
}
auto
Interpreter::EqualityVisitor::operator() (
    [[maybe_unused]] std::nullptr_t const &left,
    [[maybe_unused]] std::nullptr_t const &right) -> bool
{
  return true;
}

auto
Interpreter::StringifyVisitor::operator() (bool const &v) -> std::string
{
  return v ? "TRUE" : "FALSE";
}
auto
Interpreter::StringifyVisitor::operator() (double const &v) -> std::string
{
  return std::to_string (v);
}
auto
Interpreter::StringifyVisitor::operator() (std::string const &v) -> std::string
{
  return v;
}
auto
Interpreter::StringifyVisitor::operator() (
    [[maybe_unused]] std::nullptr_t const &v) -> std::string
{
  return "NIL";
}