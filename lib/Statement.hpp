#ifndef CPPLOX_STATEMENT_HPP
#define CPPLOX_STATEMENT_HPP

#include <optional>
#include <variant>

#include "Expression.hpp"
#include "Token.hpp"

/*
program        → declaration* EOF ;

declaration    → varDecl
               | statement ;

varDecl        → "var" IDENTIFIER ( "=" assign )? ( "," IDENTIFIER ( "=" assign )? )* ";"

statement      → exprStmt
               | printStmt ;

exprStmt       → expression ";" ;
printStmt      → "print" expression ";" ;
*/

class ExpressionStatement;
class PrintStatement;
class VariableDeclaration;

using Statement = std::variant<Box<ExpressionStatement>, Box<PrintStatement>,
                               Box<VariableDeclaration> >;

class ExpressionStatement {
  Expression m_expr;

 public:
  ExpressionStatement(Expression &&expr);

  [[nodiscard]] auto getExpression() const -> Expression const &;
};

class PrintStatement {
  Expression m_expr;

 public:
  PrintStatement(Expression &&expr);

  [[nodiscard]] auto getExpression() const -> Expression const &;
};

class VariableDeclaration {
  ValueToken<TokenType::TOKEN_IDENTIFIER> const *m_name;
  std::optional<Expression> m_initializer;

 public:
  VariableDeclaration(ValueToken<TokenType::TOKEN_IDENTIFIER> const *name,
                      std::optional<Expression> &&initializer = std::nullopt);

  [[nodiscard]] auto getName() const -> std::string_view;
  [[nodiscard]] auto getInitializer() const -> std::optional<Expression>;
};

#endif /* CPPLOX_STATEMENT_HPP */
