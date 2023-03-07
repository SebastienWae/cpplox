#ifndef CPPLOX_STATEMENT_HPP
#define CPPLOX_STATEMENT_HPP

#include "Expression.hpp"
#include <variant>

/*
program        → statement* EOF ;

statement      → exprStmt
               | printStmt ;

exprStmt       → expression ";" ;
printStmt      → "print" expression ";" ;
*/

class ExpressionStatement;
class PrintStatement;

using Statement = std::variant<ExpressionStatement, PrintStatement>;

class ExpressionStatement
{
  Expression const m_expr;

public:
  ExpressionStatement (Expression &&expr);

  [[nodiscard]] auto getExpression () const -> Expression const &;
};

class PrintStatement
{
  Expression const m_expr;

public:
  PrintStatement (Expression &&expr);

  [[nodiscard]] auto getExpression () const -> Expression const &;
};

#endif /* CPPLOX_STATEMENT_HPP */
