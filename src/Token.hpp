#ifndef CPPLOX_TOKEN_HPP
#define CPPLOX_TOKEN_HPP

#include "utils/Enums.hpp"
#include <fmt/core.h>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

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
  TokenType const m_type;
  std::uint32_t const m_offset;
  std::uint32_t const m_length;
  std::uint32_t const m_line;

public:
  Token(TokenType type, std::uint32_t offset, std::uint32_t length,
        std::uint32_t line);

  virtual ~Token() = default;

  [[nodiscard]] inline auto getType() const -> TokenType { return m_type; }
  [[nodiscard]] inline auto getOffset() const -> std::uint32_t {
    return m_offset;
  }
  [[nodiscard]] inline auto getLength() const -> std::uint32_t {
    return m_length;
  }
  [[nodiscard]] inline auto getLine() const -> std::uint32_t { return m_line; }

  [[nodiscard]] virtual auto toString() const -> std::string = 0;
};

template <TokenType type> constexpr auto is_value_token() -> bool {
  return type == TokenType::TOKEN_NUMBER || type == TokenType::TOKEN_STRING ||
         type == TokenType::TOKEN_IDENTIFIER;
}

template <TokenType type>
  requires(!is_value_token<type>())
class BasicToken : public Token {
public:
  BasicToken(std::uint32_t offset, std::uint32_t length, std::uint32_t line)
      : Token(type, offset, length, line) {}

  [[nodiscard]] auto toString() const -> std::string override {
    return fmt::format("[{}]\n - offset: {}\n - length: {}\n - line: {}",
                       Enums::enum_to_string(getType()), getOffset(),
                       getLength(), getLine());
  }
};

using TokenValueType = std::variant<double, std::string_view>;

template <TokenType type> class ValueToken : public Token {
  TokenValueType const m_value;

public:
  [[nodiscard]] auto toString() const -> std::string override {
    std::string value_string;

    if (auto const *ptr_val = std::get_if<double>(&m_value)) {
      value_string = fmt::format("{}", *ptr_val);
    } else if (auto const *ptr_val = std::get_if<std::string_view>(&m_value)) {
      value_string = *ptr_val;
    }

    return fmt::format(
        "[{}]\n - value: {}\n - offset: {}\n - length: {}\n - line: {}",
        Enums::enum_to_string(getType()), value_string, getOffset(),
        getLength(), getLine());
  }

  // NUMBER
  ValueToken(double value, std::uint32_t offset, std::uint32_t length,
             std::uint32_t line)
    requires(type == TokenType::TOKEN_NUMBER)
      : Token(type, offset, length, line), m_value(value) {}

  [[nodiscard]] inline auto getValue() const -> double
    requires(type == TokenType::TOKEN_NUMBER)
  {
    return std::get<double>(m_value);
  }

  // STRING
  ValueToken(std::string_view value, std::uint32_t offset, std::uint32_t length,
             std::uint32_t line)
    requires(type == TokenType::TOKEN_STRING)
      : Token(type, offset, length, line), m_value(value) {}

  [[nodiscard]] inline auto getValue() const -> std::string_view
    requires(type == TokenType::TOKEN_STRING)
  {
    return std::get<std::string_view>(m_value);
  }

  // IDENTIFIER
  ValueToken(std::string_view value, std::uint32_t offset, std::uint32_t length,
             std::uint32_t line)
    requires(type == TokenType::TOKEN_IDENTIFIER)
      : Token(type, offset, length, line), m_value(value) {}

  [[nodiscard]] inline auto getValue() const -> std::string_view
    requires(type == TokenType::TOKEN_IDENTIFIER)
  {
    return std::get<std::string_view>(m_value);
  }
};

#endif /* CPPLOX_TOKEN_HPP */