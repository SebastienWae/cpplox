#ifndef CPPLOX_PARSER_HPP
#define CPPLOX_PARSER_HPP

#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

#include "ErrorReporter.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include "Token.hpp"

class Parser {
  std::vector<std::unique_ptr<Token> > const &m_tokens;
  std::vector<std::unique_ptr<Token> >::const_iterator m_token_it;
  ErrorReporter &m_error_reporter;

 public:
  Parser(std::vector<std::unique_ptr<Token> > const &tokens,
         ErrorReporter &error_reporter);

  class ParserException : public std::runtime_error {
   public:
    ParserException(std::string const &what);
  };

  auto parse() -> std::optional<std::vector<Statement> >;

 private:
  void declarations(std::vector<Statement> &statements);

  auto variableDeclaration() -> VariableDeclaration;

  auto statement() -> Statement;

  auto printStatement() -> PrintStatement;
  auto expressionStatement() -> ExpressionStatement;
  auto blockStatement() -> BlockStatement;

  auto expression() -> Expression;
  auto assign() -> Expression;
  auto equality() -> Expression;
  auto comparison() -> Expression;
  auto term() -> Expression;
  auto factor() -> Expression;
  auto unary() -> Expression;
  auto primary() -> Expression;

  void synchronize();

  auto match(TokenType type) -> bool;
  inline auto match(std::initializer_list<TokenType> types) -> bool;
  [[nodiscard]] auto peek() const -> std::optional<Token *>;
  [[nodiscard]] auto previous() const -> std::optional<Token *>;

  auto error(std::optional<Token const *> token, std::string const &msg)
      -> ParserException;
};

#endif /* CPPLOX_PARSER_HPP */
