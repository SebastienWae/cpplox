#include "Expression.hpp"
#include "Token.hpp"
#include <string_view>

LiteralNumberExpression::LiteralNumberExpression (double value)
    : m_value (value)
{
}

[[nodiscard]] auto
LiteralNumberExpression::getValue () const -> double
{
  return m_value;
}

LiteralStringExpression::LiteralStringExpression (std::string_view value)
    : m_value (value){};

[[nodiscard]] auto
LiteralStringExpression::getValue () const -> std::string_view
{
  return m_value;
}

VariableExpression::VariableExpression (
    ValueToken<TokenType::TOKEN_IDENTIFIER> *identifier)
    : m_identifier (identifier)
{
}

[[nodiscard]] auto
VariableExpression::getName () const -> std::string_view
{
  return m_identifier->getValue ();
}

[[nodiscard]] auto
VariableExpression::getIdentifier () const
    -> ValueToken<TokenType::TOKEN_IDENTIFIER> *
{
  return m_identifier;
}

GroupingExpression::GroupingExpression (Expression &&expr) : m_expr (expr) {}

[[nodiscard]] auto
GroupingExpression::getExpression () const -> Expression const &
{
  return m_expr;
}

TernaryExpression::TernaryExpression (Expression &&condition,
                                      Expression &&true_expr,
                                      Expression &&false_expr)
    : m_condition (condition), m_true_expr (true_expr),
      m_false_expr (false_expr)
{
}

[[nodiscard]] auto
TernaryExpression::getConditionExpression () const -> Expression const &
{
  return m_condition;
}

[[nodiscard]] auto
TernaryExpression::getTrueExpression () const -> Expression const &
{
  return m_true_expr;
}

[[nodiscard]] auto
TernaryExpression::getFalseExpression () const -> Expression const &
{
  return m_false_expr;
}

AssignExpression::AssignExpression (
    ValueToken<TokenType::TOKEN_IDENTIFIER> *identifier, Expression &&value)
    : m_identifier (identifier), m_value (value)
{
}

[[nodiscard]] auto
AssignExpression::getName () const -> std::string_view
{
  return m_identifier->getValue ();
}

[[nodiscard]] auto
AssignExpression::getValue () const -> Expression const &
{
  return m_value;
}

[[nodiscard]] auto
AssignExpression::getIdentifier () const
    -> ValueToken<TokenType::TOKEN_IDENTIFIER> *
{
  return m_identifier;
}