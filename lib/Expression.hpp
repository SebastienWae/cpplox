#ifndef CPPLOX_EXPRESSION_HPP
#define CPPLOX_EXPRESSION_HPP

#include "./utils/Box.hpp"
#include "Token.hpp"
#include <cstddef>
#include <iostream>
#include <string_view>
#include <utility>
#include <variant>

class LiteralNumberExpression;
class LiteralStringExpression;
template <TokenType type>
requires (type == TokenType::TOKEN_TRUE || type == TokenType::TOKEN_FALSE
          || type == TokenType::TOKEN_NIL) class LiteralExpression;
class GroupingExpression;
template <TokenType type>
requires (type == TokenType::TOKEN_MINUS
          || type == TokenType::TOKEN_BANG) class UnaryExpression;
template <TokenType type>
requires (type == TokenType::TOKEN_EQUAL_EQUAL
          || type == TokenType::TOKEN_BANG_EQUAL
          || type == TokenType::TOKEN_LESS
          || type == TokenType::TOKEN_LESS_EQUAL
          || type == TokenType::TOKEN_GREATER
          || type == TokenType::TOKEN_GREATER_EQUAL
          || type == TokenType::TOKEN_PLUS || type == TokenType::TOKEN_MINUS
          || type == TokenType::TOKEN_STAR || type == TokenType::TOKEN_SLASH
          || type == TokenType::TOKEN_COMMA) class BinaryExpression;
class TernaryExpression;

using Expression
    = std::variant<Box<LiteralNumberExpression>, Box<LiteralStringExpression>,
                   Box<LiteralExpression<TokenType::TOKEN_TRUE> >,
                   Box<LiteralExpression<TokenType::TOKEN_FALSE> >,
                   Box<LiteralExpression<TokenType::TOKEN_NIL> >,
                   Box<GroupingExpression>, Box<TernaryExpression>,
                   Box<UnaryExpression<TokenType::TOKEN_MINUS> >,
                   Box<UnaryExpression<TokenType::TOKEN_BANG> >,
                   Box<BinaryExpression<TokenType::TOKEN_EQUAL_EQUAL> >,
                   Box<BinaryExpression<TokenType::TOKEN_BANG_EQUAL> >,
                   Box<BinaryExpression<TokenType::TOKEN_LESS> >,
                   Box<BinaryExpression<TokenType::TOKEN_LESS_EQUAL> >,
                   Box<BinaryExpression<TokenType::TOKEN_GREATER> >,
                   Box<BinaryExpression<TokenType::TOKEN_GREATER_EQUAL> >,
                   Box<BinaryExpression<TokenType::TOKEN_PLUS> >,
                   Box<BinaryExpression<TokenType::TOKEN_MINUS> >,
                   Box<BinaryExpression<TokenType::TOKEN_STAR> >,
                   Box<BinaryExpression<TokenType::TOKEN_SLASH> >,
                   Box<BinaryExpression<TokenType::TOKEN_COMMA> > >;

class LiteralNumberExpression
{
  double const m_value;

public:
  LiteralNumberExpression (double value) : m_value (value){};

  [[nodiscard]] auto
  getValue () const -> double
  {
    return m_value;
  }
};

class LiteralStringExpression
{
  std::string_view const m_value;

public:
  LiteralStringExpression (std::string_view value) : m_value (value){};

  [[nodiscard]] auto
  getValue () const -> std::string_view
  {
    return m_value;
  }
};

template <TokenType type>
requires (type == TokenType::TOKEN_TRUE || type == TokenType::TOKEN_FALSE
          || type == TokenType::TOKEN_NIL) class LiteralExpression
{
};

class GroupingExpression
{
  Expression const m_expr;

public:
  GroupingExpression (Expression &&expr) : m_expr (expr) {}

  [[nodiscard]] auto
  getExpression () const -> Expression const &
  {
    return m_expr;
  }
};

template <TokenType type>
requires (type == TokenType::TOKEN_MINUS
          || type == TokenType::TOKEN_BANG) class UnaryExpression
{
  BasicToken<type> const *m_operator;
  Expression const m_right_expr;

public:
  UnaryExpression (BasicToken<type> const *op, Expression &&expr)
      : m_operator (op), m_right_expr (expr)
  {
  }

  [[nodiscard]] auto
  getExpression () const -> Expression const &
  {
    return m_right_expr;
  }

  auto
  getOperator () const -> BasicToken<type> const *
  {
    return m_operator;
  }
};

template <TokenType type>
requires (type == TokenType::TOKEN_EQUAL_EQUAL
          || type == TokenType::TOKEN_BANG_EQUAL
          || type == TokenType::TOKEN_LESS
          || type == TokenType::TOKEN_LESS_EQUAL
          || type == TokenType::TOKEN_GREATER
          || type == TokenType::TOKEN_GREATER_EQUAL
          || type == TokenType::TOKEN_PLUS || type == TokenType::TOKEN_MINUS
          || type == TokenType::TOKEN_STAR || type == TokenType::TOKEN_SLASH
          || type == TokenType::TOKEN_COMMA) class BinaryExpression
{
  Expression const m_left_expr;
  BasicToken<type> const *m_operator;
  Expression const m_right_expr;

public:
  BinaryExpression (Expression &&left_expr, BasicToken<type> const *op,
                    Expression &&right_expr)
      : m_left_expr (left_expr), m_operator (op), m_right_expr (right_expr)
  {
  }

  [[nodiscard]] auto
  getLeftExpression () const -> Expression const &
  {
    return m_left_expr;
  }

  [[nodiscard]] auto
  getRightExpression () const -> Expression const &
  {
    return m_right_expr;
  }

  auto
  getOperator () const -> BasicToken<type> const *
  {
    return m_operator;
  }
};

class TernaryExpression
{
  Expression const m_condition;
  Expression const m_true_expr;
  Expression const m_false_expr;

public:
  TernaryExpression (Expression &&condition, Expression &&true_expr,
                     Expression &&false_expr)
      : m_condition (condition), m_true_expr (true_expr),
        m_false_expr (false_expr)
  {
  }

  [[nodiscard]] auto
  getConditionExpression () const -> Expression const &
  {
    return m_condition;
  }
  [[nodiscard]] auto
  getTrueExpression () const -> Expression const &
  {
    return m_true_expr;
  }
  [[nodiscard]] auto
  getFalseExpression () const -> Expression const &
  {
    return m_false_expr;
  }
};

#endif /* CPPLOX_EXPRESSION_HPP */
