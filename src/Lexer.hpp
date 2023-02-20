#ifndef CPPLOX_LEXER_HPP
#define CPPLOX_LEXER_HPP

#include "ErrorReporter.hpp"
#include "Token.hpp"
#include <functional>
#include <memory>
#include <string_view>
#include <type_traits>
#include <vector>

class Lexer {
  std::vector<std::unique_ptr<Token>> m_tokens;
  std::string_view m_source;
  ErrorReporter &m_error_reporter;

  std::uint32_t m_pos_start = 0;
  std::uint32_t m_pos_current = 0;
  std::uint32_t m_line_start = 1;
  std::uint32_t m_line_current = 1;

public:
  Lexer(std::string_view source, ErrorReporter &error_reporter);

  auto scanTokens() -> std::optional<
      std::reference_wrapper<std::vector<std::unique_ptr<Token>>>>;

private:
  [[nodiscard]] auto isAtEnd() const -> bool;

  void scanToken();

  template <TokenType type>
    requires(!is_value_token<type>())
  void addToken() {
    m_tokens.emplace_back(std::make_unique<BasicToken<type>>(
        m_pos_start, m_pos_current - m_pos_start, m_line_start));
  }

  template <TokenType type>
    requires(type == TokenType::TOKEN_NUMBER)
  void addToken(double value) {
    m_tokens.emplace_back(std::make_unique<ValueToken<type>>(
        value, m_pos_start, m_pos_current - m_pos_start, m_line_start));
  }

  template <TokenType type>
    requires(type == TokenType::TOKEN_STRING)
  void addToken(std::string_view value) {
    m_tokens.emplace_back(std::make_unique<ValueToken<type>>(
        value, m_pos_start, m_pos_current - m_pos_start, m_line_start));
  }

  template <TokenType type>
    requires(type == TokenType::TOKEN_IDENTIFIER)
  void addToken(std::string_view value) {
    m_tokens.emplace_back(std::make_unique<ValueToken<type>>(
        value, m_pos_start, m_pos_current - m_pos_start, m_line_start));
  }

  auto match(char const &expected) -> bool;
  [[nodiscard]] auto peek() const -> char;
  [[nodiscard]] auto peekNext() const -> char;

  void string();
  void number();
  void identifier();
};

#endif /* CPPLOX_LEXER_HPP */