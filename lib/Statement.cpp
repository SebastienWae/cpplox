#include "Statement.hpp"
#include "Expression.hpp"

ExpressionStatement::ExpressionStatement (Expression &&expr) : m_expr (expr) {}

[[nodiscard]] auto
ExpressionStatement::getExpression () const -> Expression const &
{
  return m_expr;
}

PrintStatement::PrintStatement (Expression &&expr) : m_expr (expr){};

[[nodiscard]] auto
PrintStatement ::getExpression () const -> Expression const &
{
  return m_expr;
}