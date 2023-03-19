#include "Statement.hpp"

#include "Expression.hpp"

ExpressionStatement::ExpressionStatement(Expression&& expr) : m_expr(expr) {}

[[nodiscard]] auto ExpressionStatement::getExpression() const
    -> Expression const& {
  return m_expr;
}

PrintStatement::PrintStatement(Expression&& expr) : m_expr(expr){};

[[nodiscard]] auto PrintStatement ::getExpression() const -> Expression const& {
  return m_expr;
}

VariableDeclaration::VariableDeclaration(
    ValueToken<TokenType::TOKEN_IDENTIFIER> const* name,
    std::optional<Expression>&& initializer)
    : m_name(name), m_initializer(initializer) {}

[[nodiscard]] auto VariableDeclaration::getName() const -> std::string_view {
  return m_name->getValue();
}

[[nodiscard]] auto VariableDeclaration::getInitializer() const
    -> std::optional<Expression> {
  return m_initializer;
}

BlockStatement::BlockStatement(std::vector<Statement>&& stmts)
    : m_stmts(stmts) {}

[[nodiscard]] auto BlockStatement::getStatements() const
    -> std::vector<Statement> const& {
  return m_stmts;
}