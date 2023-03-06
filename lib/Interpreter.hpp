#ifndef CPPLOX_INTERPRETER_HPP
#define CPPLOX_INTERPRETER_HPP

#include "ErrorReporter.hpp"
#include "Expression.hpp"
#include <cstddef>
#include <string>

class Interpreter
{
public:
  using ValueType = std::variant<bool, double, std::string, std::nullptr_t>;

private:
  struct Visitor
  {
    Visitor (Interpreter &interpreter) : m_interpreter (interpreter) {}

    auto operator() (Box<LiteralNumberExpression> const &e) -> ValueType;
    auto operator() ([[maybe_unused]] Box<LiteralStringExpression> const &e)
        -> ValueType;
    auto operator() (
        [[maybe_unused]] Box<LiteralExpression<TokenType::TOKEN_TRUE> > const
            &e) -> ValueType;

    auto operator() (
        [[maybe_unused]] Box<LiteralExpression<TokenType::TOKEN_FALSE> > const
            &e) -> ValueType;
    auto operator() (
        [[maybe_unused]] Box<LiteralExpression<TokenType::TOKEN_NIL> > const
            &e) -> ValueType;
    auto operator() (Box<GroupingExpression> const &e) -> ValueType;
    auto operator() (Box<TernaryExpression> const &e) -> ValueType;
    auto operator() (Box<UnaryExpression<TokenType::TOKEN_MINUS> > const &e)
        -> ValueType;
    auto operator() (Box<UnaryExpression<TokenType::TOKEN_BANG> > const &e)
        -> ValueType;
    auto
    operator() (Box<BinaryExpression<TokenType::TOKEN_EQUAL_EQUAL> > const &e)
        -> ValueType;
    auto
    operator() (Box<BinaryExpression<TokenType::TOKEN_BANG_EQUAL> > const &e)
        -> ValueType;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_LESS> > const &e)
        -> ValueType;
    auto
    operator() (Box<BinaryExpression<TokenType::TOKEN_LESS_EQUAL> > const &e)
        -> ValueType;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_GREATER> > const &e)
        -> ValueType;
    auto operator() (
        Box<BinaryExpression<TokenType::TOKEN_GREATER_EQUAL> > const &e)
        -> ValueType;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_PLUS> > const &e)
        -> ValueType;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_MINUS> > const &e)
        -> ValueType;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_STAR> > const &e)
        -> ValueType;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_SLASH> > const &e)
        -> ValueType;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_COMMA> > const &e)
        -> ValueType;

  private:
    Interpreter &m_interpreter;
  };

  struct TruthyVisitor
  {
    auto operator() (bool const &v) -> bool;
    auto operator() ([[maybe_unused]] double const &v) -> bool;
    auto operator() ([[maybe_unused]] std::string const &v) -> bool;
    auto operator() ([[maybe_unused]] std::nullptr_t const &v) -> bool;
  };

  struct EqualityVisitor
  {
    auto operator() (bool const &left, bool const &right) -> bool;
    auto operator() ([[maybe_unused]] bool const &left,
                     [[maybe_unused]] double const &right) -> bool;
    auto operator() ([[maybe_unused]] bool const &left,
                     [[maybe_unused]] std::string const &right) -> bool;
    auto operator() ([[maybe_unused]] bool const &left,
                     [[maybe_unused]] std::nullptr_t const &right) -> bool;

    auto operator() ([[maybe_unused]] double const &left,
                     [[maybe_unused]] bool const &right) -> bool;
    auto operator() (double const &left, double const &right) -> bool;
    auto operator() ([[maybe_unused]] double const &left,
                     [[maybe_unused]] std::string const &right) -> bool;
    auto operator() ([[maybe_unused]] double const &left,
                     [[maybe_unused]] std::nullptr_t const &right) -> bool;

    auto operator() ([[maybe_unused]] std::string const &left,
                     [[maybe_unused]] bool const &right) -> bool;
    auto operator() ([[maybe_unused]] std::string const &left,
                     [[maybe_unused]] double const &right) -> bool;
    auto operator() (std::string const &left, std::string const &right)
        -> bool;
    auto operator() ([[maybe_unused]] std::string const &left,
                     [[maybe_unused]] std::nullptr_t const &right) -> bool;

    auto operator() ([[maybe_unused]] std::nullptr_t const &left,
                     [[maybe_unused]] bool const &right) -> bool;
    auto operator() ([[maybe_unused]] std::nullptr_t const &left,
                     [[maybe_unused]] double const &right) -> bool;
    auto operator() ([[maybe_unused]] std::nullptr_t const &left,
                     [[maybe_unused]] std::string const &right) -> bool;
    auto operator() ([[maybe_unused]] std::nullptr_t const &left,
                     [[maybe_unused]] std::nullptr_t const &right) -> bool;
  };

  struct StringifyVisitor
  {
    auto operator() (bool const &v) -> std::string;
    auto operator() ([[maybe_unused]] double const &v) -> std::string;
    auto operator() ([[maybe_unused]] std::string const &v) -> std::string;
    auto operator() ([[maybe_unused]] std::nullptr_t const &v) -> std::string;
  };

  Expression const &m_expression;
  ErrorReporter &m_error_reporter;

  Visitor m_visitor;

public:
  Interpreter (Expression const &expression, ErrorReporter &error_reporter);

  class InterpreterException : public std::runtime_error
  {
  public:
    InterpreterException (std::string const &what);
  };

  auto interpret () -> std::optional<std::string const>;

private:
  static auto isTruthy (ValueType const &value) -> bool;
  static auto isEqual (ValueType const &left_value,
                       ValueType const &right_value) -> bool;

  auto error (std::optional<Token const *> token, std::string const &msg)
      -> InterpreterException;
};

#endif /* CPPLOX_INTERPRETER_HPP */
