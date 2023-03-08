#ifndef CPPLOX_INTERPRETER_HPP
#define CPPLOX_INTERPRETER_HPP

#include "ErrorReporter.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include <cstddef>
#include <string>
#include <vector>

class Environment;

class Interpreter
{
public:
  using ExpressionValue
      = std::variant<bool, double, std::string, std::nullptr_t>;

private:
  struct StatementVisitor
  {
    StatementVisitor (Interpreter &interpreter);

    auto operator() (Box<PrintStatement> const &s) -> void;
    auto operator() (Box<ExpressionStatement> const &s) -> void;
    auto operator() (Box<VariableDeclaration> const &s) -> void;

  private:
    Interpreter &m_interpreter;
  };

  struct ExpressionVisitor
  {
    ExpressionVisitor (Interpreter &interpreter);

    auto operator() (Box<LiteralNumberExpression> const &e) -> ExpressionValue;
    auto operator() ([[maybe_unused]] Box<LiteralStringExpression> const &e)
        -> ExpressionValue;
    auto operator() (
        [[maybe_unused]] Box<LiteralExpression<TokenType::TOKEN_TRUE> > const
            &e) -> ExpressionValue;
    auto operator() (
        [[maybe_unused]] Box<LiteralExpression<TokenType::TOKEN_FALSE> > const
            &e) -> ExpressionValue;
    auto operator() (
        [[maybe_unused]] Box<LiteralExpression<TokenType::TOKEN_NIL> > const
            &e) -> ExpressionValue;
    auto operator() (Box<VariableExpression> const &e) -> ExpressionValue;
    auto operator() (Box<GroupingExpression> const &e) -> ExpressionValue;
    auto operator() (Box<TernaryExpression> const &e) -> ExpressionValue;
    auto operator() (Box<AssignExpression> const &e) -> ExpressionValue;
    auto operator() (Box<UnaryExpression<TokenType::TOKEN_MINUS> > const &e)
        -> ExpressionValue;
    auto operator() (Box<UnaryExpression<TokenType::TOKEN_BANG> > const &e)
        -> ExpressionValue;
    auto
    operator() (Box<BinaryExpression<TokenType::TOKEN_EQUAL_EQUAL> > const &e)
        -> ExpressionValue;
    auto
    operator() (Box<BinaryExpression<TokenType::TOKEN_BANG_EQUAL> > const &e)
        -> ExpressionValue;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_LESS> > const &e)
        -> ExpressionValue;
    auto
    operator() (Box<BinaryExpression<TokenType::TOKEN_LESS_EQUAL> > const &e)
        -> ExpressionValue;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_GREATER> > const &e)
        -> ExpressionValue;
    auto operator() (
        Box<BinaryExpression<TokenType::TOKEN_GREATER_EQUAL> > const &e)
        -> ExpressionValue;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_PLUS> > const &e)
        -> ExpressionValue;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_MINUS> > const &e)
        -> ExpressionValue;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_STAR> > const &e)
        -> ExpressionValue;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_SLASH> > const &e)
        -> ExpressionValue;
    auto operator() (Box<BinaryExpression<TokenType::TOKEN_COMMA> > const &e)
        -> ExpressionValue;

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

  std::vector<Statement> const &m_statements;
  std::unique_ptr<Environment> m_environment;

  ErrorReporter &m_error_reporter;

public:
  Interpreter (std::vector<Statement> const &statements,
               ErrorReporter &error_reporter);
  Interpreter (const Interpreter &) = delete;
  Interpreter (Interpreter &&) = delete;
  auto operator= (const Interpreter &) -> Interpreter & = delete;
  auto operator= (Interpreter &&) -> Interpreter & = delete;
  ~Interpreter ();

  class InterpreterException : public std::runtime_error
  {
  public:
    InterpreterException (std::string const &what);
  };

  void interpret ();

private:
  static auto isTruthy (ExpressionValue const &value) -> bool;
  static auto isEqual (ExpressionValue const &left_value,
                       ExpressionValue const &right_value) -> bool;

  auto error (std::optional<Token const *> token, std::string const &msg)
      -> InterpreterException;
};

#endif /* CPPLOX_INTERPRETER_HPP */
