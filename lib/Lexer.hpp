#ifndef CPPLOX_LEXER_HPP
#define CPPLOX_LEXER_HPP

#include <functional>
#include <memory>
#include <string_view>
#include <type_traits>
#include <vector>

#include "ErrorReporter.hpp"
#include "SourcePosition.hpp"
#include "Token.hpp"

class Lexer {
  std::vector<std::unique_ptr<Token> > m_tokens;
  std::string_view m_source;
  ErrorReporter &m_error_reporter;

  std::uint32_t m_prev_cursor = 0;
  std::uint32_t m_cur_cursor = 0;

  std::uint32_t m_start_line = 1;
  std::uint32_t m_current_line = m_start_line;
  std::uint32_t m_start_line_position = 0;
  std::uint32_t m_current_line_position = 0;

 public:
  Lexer(std::string_view source, ErrorReporter &error_reporter);

  auto scanTokens() -> std::optional<
      std::reference_wrapper<std::vector<std::unique_ptr<Token> > > >;

 private:
  [[nodiscard]] auto isAtEnd() const -> bool;

  void nextLine();
  void moveCursor(std::uint32_t i);

  void scanToken();

  template <TokenType type>
    requires(!is_value_token<type>())
  void addToken() {
    m_tokens.emplace_back(std::make_unique<BasicToken<type> >(
        SourcePosition(m_start_line, m_current_line, m_start_line_position,
                       m_current_line_position - 1)));
  }

  template <TokenType type>
    requires(type == TokenType::TOKEN_NUMBER)
  void addToken(double value) {
    m_tokens.emplace_back(std::make_unique<ValueToken<type> >(
        value,
        SourcePosition(m_start_line, m_current_line, m_start_line_position,
                       m_current_line_position - 1)));
  }

  template <TokenType type>
    requires(type == TokenType::TOKEN_STRING)
  void addToken(std::string_view value) {
    m_tokens.emplace_back(std::make_unique<ValueToken<type> >(
        value,
        SourcePosition(m_start_line, m_current_line, m_start_line_position,
                       m_current_line_position - 1)));
  }

  template <TokenType type>
    requires(type == TokenType::TOKEN_IDENTIFIER)
  void addToken(std::string_view value) {
    m_tokens.emplace_back(std::make_unique<ValueToken<type> >(
        value,
        SourcePosition(m_start_line, m_current_line, m_start_line_position,
                       m_current_line_position - 1)));
  }

  [[nodiscard]] auto match(char c) -> bool;
  [[nodiscard]] auto peek() const -> char;
  [[nodiscard]] auto peekNext() const -> char;

  void string();
  void number();
  void identifier();

  void error(std::string const &error_msg);
};

#endif /* CPPLOX_LEXER_HPP */