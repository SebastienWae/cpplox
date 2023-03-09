#ifndef CPPLOX_TOKEN_HPP
#define CPPLOX_TOKEN_HPP

#include <fmt/core.h>

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include "utils/Enums.hpp"

enum class TokenType {
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_NUMBER,

  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_COMMA,
  TOKEN_DOT,
  TOKEN_MINUS,
  TOKEN_PLUS,
  TOKEN_SEMICOLON,
  TOKEN_SLASH,
  TOKEN_STAR,

  TOKEN_QUESTION,
  TOKEN_COLON,

  TOKEN_BANG,
  TOKEN_BANG_EQUAL,
  TOKEN_EQUAL,
  TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER,
  TOKEN_GREATER_EQUAL,
  TOKEN_LESS,
  TOKEN_LESS_EQUAL,

  TOKEN_AND,
  TOKEN_CLASS,
  TOKEN_ELSE,
  TOKEN_FALSE,
  TOKEN_FUN,
  TOKEN_FOR,
  TOKEN_IF,
  TOKEN_NIL,
  TOKEN_OR,
  TOKEN_PRINT,
  TOKEN_RETURN,
  TOKEN_SUPER,
  TOKEN_THIS,
  TOKEN_TRUE,
  TOKEN_VAR,
  TOKEN_WHILE,

  TOKEN_EOF
};

class Token {
  std::uint32_t const m_offset;
  std::uint32_t const m_length;
  std::uint32_t const m_line;

 protected:
  Token(std::uint32_t offset, std::uint32_t length, std::uint32_t line);

 public:
  virtual ~Token() = default;
  Token(const Token&) = delete;
  Token(Token&&) = delete;
  auto operator=(const Token&) -> Token& = delete;
  auto operator=(Token&&) -> Token& = delete;

  [[nodiscard]] inline auto getOffset() const -> std::uint32_t {
    return m_offset;
  }
  [[nodiscard]] inline auto getLength() const -> std::uint32_t {
    return m_length;
  }
  [[nodiscard]] inline auto getLine() const -> std::uint32_t { return m_line; }

  [[nodiscard]] virtual auto getType() const -> TokenType = 0;
  [[nodiscard]] virtual auto toString() const -> std::string = 0;
};

template <TokenType type>
constexpr auto is_value_token() -> bool {
  return type == TokenType::TOKEN_NUMBER || type == TokenType::TOKEN_STRING ||
         type == TokenType::TOKEN_IDENTIFIER;
}

template <TokenType type>
  requires(!is_value_token<type>())
class BasicToken : public Token {
 public:
  BasicToken(std::uint32_t offset, std::uint32_t length, std::uint32_t line)
      : Token(offset, length, line) {}

  [[nodiscard]] auto getType() const -> TokenType override { return type; }

  [[nodiscard]] inline auto toString() const -> std::string override {
    return fmt::format("[{}]\n - offset: {}\n - length: {}\n - line: {}",
                       Enums::enum_to_string(type), getOffset(), getLength(),
                       getLine());
  }
};

template <TokenType type>
class ValueToken;

template <>
class ValueToken<TokenType::TOKEN_NUMBER> : public Token {
  double const m_value;

 public:
  ValueToken(double value, std::uint32_t offset, std::uint32_t length,
             std::uint32_t line)
      : Token(offset, length, line), m_value(value) {}

  [[nodiscard]] auto getType() const -> TokenType override {
    return TokenType::TOKEN_NUMBER;
  }

  [[nodiscard]] inline auto toString() const -> std::string override {
    return fmt::format(
        "[{}]\n - value: {}\n - offset: {}\n - length: {}\n - line: {}",
        Enums::enum_to_string(TokenType::TOKEN_NUMBER), m_value, getOffset(),
        getLength(), getLine());
  }

  [[nodiscard]] inline auto getValue() const -> double { return m_value; }
};

template <TokenType type>
  requires(type == TokenType::TOKEN_STRING ||
           type == TokenType::TOKEN_IDENTIFIER)
class ValueToken<type> : public Token {
  std::string_view const m_value;

 public:
  ValueToken(std::string_view value, std::uint32_t offset, std::uint32_t length,
             std::uint32_t line)
      : Token(offset, length, line), m_value(value) {}

  [[nodiscard]] auto getType() const -> TokenType override { return type; }

  [[nodiscard]] inline auto toString() const -> std::string override {
    return fmt::format(
        "[{}]\n - value: {}\n - offset: {}\n - length: {}\n - line: {}",
        Enums::enum_to_string(type), m_value, getOffset(), getLength(),
        getLine());
  }

  [[nodiscard]] inline auto getValue() const -> std::string_view {
    return m_value;
  }
};

#endif /* CPPLOX_TOKEN_HPP */
