#include "Expression.hpp"

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