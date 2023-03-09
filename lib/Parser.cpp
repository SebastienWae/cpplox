#include "Parser.hpp"

#include <algorithm>
#include <iterator>
#include <memory>
#include <optional>
#include <stdexcept>
#include <variant>
#include <vector>

#include "Expression.hpp"
#include "Statement.hpp"
#include "Token.hpp"

Parser::Parser(std::vector<std::unique_ptr<Token> > const &tokens,
               ErrorReporter &error_reporter)
    : m_tokens(tokens),
      m_token_it(tokens.begin()),
      m_error_reporter(error_reporter) {}

auto Parser::parse() -> std::optional<std::vector<Statement> > {
  try {
    std::vector<Statement> statements;

    auto it_end = m_tokens.end();
    while (m_token_it != it_end) {
      declarations(statements);
    }

    return statements;
  } catch (Parser::ParserException &e) {
    return std::nullopt;
  } catch (...) {
    m_error_reporter.setError("Parser", "Unexpected error");
    return std::nullopt;
  }
}

void Parser::declarations(std::vector<Statement> &statements) {
  try {
    if (match(TokenType::TOKEN_VAR)) {
      statements.emplace_back(variableDeclaration());
      while (match(TokenType::TOKEN_COMMA)) {
        statements.emplace_back(variableDeclaration());
      }
    } else {
      statements.emplace_back(statement());
    }
  } catch (Parser::ParserException &e) {
    synchronize();
  }
}

auto Parser::variableDeclaration() -> VariableDeclaration {
  if (!match(TokenType::TOKEN_IDENTIFIER)) {
    throw error(peek(), "Variable name expected");
  }

  auto *id = static_cast<ValueToken<TokenType::TOKEN_IDENTIFIER> *>(
      previous().value());

  if (match(TokenType::TOKEN_EQUAL)) {
    auto initializer = assign();
    if (match({TokenType::TOKEN_SEMICOLON, TokenType::TOKEN_COMMA})) {
      return {id, std::move(initializer)};
    }
  } else if (match(TokenType::TOKEN_SEMICOLON)) {
    return {id};
  }

  throw error(peek(), "';' expected after value");
}

auto Parser::statement() -> Statement {
  if (match(TokenType::TOKEN_PRINT)) {
    return printStatement();
  }

  return expressionStatement();
}

auto Parser::printStatement() -> PrintStatement {
  auto value = expression();

  if (!match(TokenType::TOKEN_SEMICOLON)) {
    throw error(peek(), "';' expected after value");
  }

  return value;
}

auto Parser::expressionStatement() -> ExpressionStatement {
  auto value = expression();

  if (!match(TokenType::TOKEN_SEMICOLON)) {
    throw error(peek(), "';' expected after value");
  }

  return value;
}

auto Parser::expression() -> Expression {
  auto expr = assign();

  while (match(TokenType::TOKEN_COMMA)) {
    auto *op = previous().value();

    expr = BinaryExpression<TokenType::TOKEN_COMMA>{
        std::move(expr), static_cast<BasicToken<TokenType::TOKEN_COMMA> *>(op),
        expression()};
  }

  return expr;
}

auto Parser::assign() -> Expression {
  auto expr = equality();

  if (match(TokenType::TOKEN_EQUAL)) {
    if (auto *expr_ptr = std::get_if<Box<VariableExpression> >(&expr)) {
      auto value = assign();
      return AssignExpression{(*expr_ptr)->getIdentifier(), std::move(value)};
    }

    throw error(peek(), "Invalid assignment target");
  }

  while (match(TokenType::TOKEN_QUESTION)) {
    auto left = assign();

    if (match(TokenType::TOKEN_COLON)) {
      expr = TernaryExpression{std::move(expr), std::move(left), assign()};
    } else {
      throw error(peek(), "':' expected after expression");
    }
  }

  return expr;
}

auto Parser::equality() -> Expression {
  auto expr = comparison();

  while (match({TokenType::TOKEN_BANG_EQUAL, TokenType::TOKEN_EQUAL_EQUAL})) {
    auto *op = previous().value();
    auto right = comparison();

    if (op->getType() == TokenType::TOKEN_BANG_EQUAL) {
      expr = BinaryExpression<TokenType::TOKEN_BANG_EQUAL>{
          std::move(expr),
          static_cast<BasicToken<TokenType::TOKEN_BANG_EQUAL> *>(op),
          std::move(right)};
    } else if (op->getType() == TokenType::TOKEN_EQUAL_EQUAL) {
      expr = BinaryExpression<TokenType::TOKEN_EQUAL_EQUAL>{
          std::move(expr),
          static_cast<BasicToken<TokenType::TOKEN_EQUAL_EQUAL> *>(op),
          std::move(right)};
    }
  }

  return expr;
}

auto Parser::comparison() -> Expression {
  auto expr = term();

  while (match({TokenType::TOKEN_GREATER, TokenType::TOKEN_GREATER_EQUAL,
                TokenType::TOKEN_LESS, TokenType::TOKEN_LESS_EQUAL})) {
    auto *op = previous().value();
    auto right = term();

    if (op->getType() == TokenType::TOKEN_GREATER) {
      expr = BinaryExpression<TokenType::TOKEN_GREATER>{
          std::move(expr),
          static_cast<BasicToken<TokenType::TOKEN_GREATER> *>(op),
          std::move(right)};
    } else if (op->getType() == TokenType::TOKEN_GREATER_EQUAL) {
      expr = BinaryExpression<TokenType::TOKEN_GREATER_EQUAL>{
          std::move(expr),
          static_cast<BasicToken<TokenType::TOKEN_GREATER_EQUAL> *>(op),
          std::move(right)};
    } else if (op->getType() == TokenType::TOKEN_LESS) {
      expr = BinaryExpression<TokenType::TOKEN_LESS>{
          std::move(expr),

          static_cast<BasicToken<TokenType::TOKEN_LESS> *>(op),
          std::move(right)};
    } else if (op->getType() == TokenType::TOKEN_LESS_EQUAL) {
      expr = BinaryExpression<TokenType::TOKEN_LESS_EQUAL>{
          std::move(expr),
          static_cast<BasicToken<TokenType::TOKEN_LESS_EQUAL> *>(op),
          std::move(right)};
    }
  }

  return expr;
}

auto Parser::term() -> Expression {
  auto expr = factor();

  while (match({TokenType::TOKEN_MINUS, TokenType::TOKEN_PLUS})) {
    auto *op = previous().value();
    auto right = factor();

    if (op->getType() == TokenType::TOKEN_MINUS) {
      expr = BinaryExpression<TokenType::TOKEN_MINUS>{
          std::move(expr),
          static_cast<BasicToken<TokenType::TOKEN_MINUS> *>(op),
          std::move(right)};
    } else if (op->getType() == TokenType::TOKEN_PLUS) {
      expr = BinaryExpression<TokenType::TOKEN_PLUS>{
          std::move(expr), static_cast<BasicToken<TokenType::TOKEN_PLUS> *>(op),
          std::move(right)};
    }
  }

  return expr;
}

auto Parser::factor() -> Expression {
  auto expr = unary();

  while (match({TokenType::TOKEN_SLASH, TokenType::TOKEN_STAR})) {
    auto *op = previous().value();
    auto right = unary();

    if (op->getType() == TokenType::TOKEN_SLASH) {
      expr = BinaryExpression<TokenType::TOKEN_SLASH>{
          std::move(expr),
          static_cast<BasicToken<TokenType::TOKEN_SLASH> *>(op),
          std::move(right)};
    } else if (op->getType() == TokenType::TOKEN_STAR) {
      expr = BinaryExpression<TokenType::TOKEN_STAR>{
          std::move(expr), static_cast<BasicToken<TokenType::TOKEN_STAR> *>(op),
          std::move(right)};
    }
  }

  return expr;
}

auto Parser::unary() -> Expression {
  while (match({TokenType::TOKEN_BANG, TokenType::TOKEN_MINUS})) {
    auto *op = previous().value();
    auto right = unary();

    if (op->getType() == TokenType::TOKEN_BANG) {
      return UnaryExpression<TokenType::TOKEN_BANG>{
          static_cast<BasicToken<TokenType::TOKEN_BANG> *>(op),
          std::move(right)};
    }

    if (op->getType() == TokenType::TOKEN_MINUS) {
      return UnaryExpression<TokenType::TOKEN_MINUS>{
          static_cast<BasicToken<TokenType::TOKEN_MINUS> *>(op),
          std::move(right)};
    }
  }

  return primary();
}

auto Parser::primary() -> Expression {
  if (match(TokenType::TOKEN_FALSE)) {
    return LiteralExpression<TokenType::TOKEN_FALSE>{};
  }

  if (match(TokenType::TOKEN_TRUE)) {
    return LiteralExpression<TokenType::TOKEN_TRUE>{};
  }

  if (match(TokenType::TOKEN_NIL)) {
    return LiteralExpression<TokenType::TOKEN_NIL>{};
  }

  if (match({TokenType::TOKEN_NUMBER, TokenType::TOKEN_STRING})) {
    auto *op = previous().value();
    if (op->getType() == TokenType::TOKEN_NUMBER) {
      return LiteralNumberExpression{
          (static_cast<ValueToken<TokenType::TOKEN_NUMBER> *>(op)->getValue())};
    }
    if (op->getType() == TokenType::TOKEN_STRING) {
      return LiteralStringExpression{
          (static_cast<ValueToken<TokenType::TOKEN_STRING> *>(op)->getValue())};
    }
  }

  if (match(TokenType::TOKEN_IDENTIFIER)) {
    return VariableExpression{
        static_cast<ValueToken<TokenType::TOKEN_IDENTIFIER> *>(
            previous().value())};
  }

  if (match(TokenType::TOKEN_LEFT_PAREN)) {
    auto expr = expression();
    if (!match(TokenType::TOKEN_RIGHT_PAREN)) {
      throw error(peek(), "')' expected after expression");
    }
    return GroupingExpression{std::move(expr)};
  }

  throw error(peek(), "Expression expected");
}

void Parser::synchronize() {
  ++m_token_it;

  std::optional<Token const *> token = peek();
  while (token.has_value()) {
    std::optional<Token const *> prev_token = previous();
    if (prev_token.has_value() &&
        prev_token.value()->getType() == TokenType::TOKEN_SEMICOLON) {
      return;
    }

    switch (token.value()->getType()) {
      case TokenType::TOKEN_CLASS:
      case TokenType::TOKEN_FUN:
      case TokenType::TOKEN_VAR:
      case TokenType::TOKEN_FOR:
      case TokenType::TOKEN_IF:
      case TokenType::TOKEN_WHILE:
      case TokenType::TOKEN_PRINT:
      case TokenType::TOKEN_RETURN:
        return;
    }

    ++m_token_it;
    token = peek();
  }
}

auto Parser::match(TokenType type) -> bool {
  std::optional<Token const *> token = peek();
  if (token.has_value() && token.value()->getType() == type) {
    ++m_token_it;
    return true;
  }

  return false;
}

inline auto Parser::match(std::initializer_list<TokenType> types) -> bool {
  return std::ranges::any_of(types,
                             [this](TokenType type) { return match(type); });
}

[[nodiscard]] auto Parser::peek() const -> std::optional<Token *> {
  if (m_token_it != m_tokens.end()) {
    return m_token_it->get();
  }
  return std::nullopt;
}

[[nodiscard]] auto Parser::previous() const -> std::optional<Token *> {
  if (m_token_it != m_tokens.begin()) {
    return (m_token_it - 1)->get();
  }
  return std::nullopt;
}

auto Parser::error(std::optional<Token const *> token, std::string const &msg)
    -> ParserException {
  std::string str;
  if (!token.has_value()) {
    str = fmt::format("{} at end", msg);
  } else {
    auto const *token_value = token.value();
    str = fmt::format("Line: {}\n{}", token_value->getLine(), msg);
  }
  m_error_reporter.setError("Parser", str);
  return {str};
}

Parser::ParserException::ParserException(std::string const &what)
    : std::runtime_error("[ParserException]\n" + what) {}