#include "Lexer.hpp"

#include <cctype>
#include <charconv>
#include <cstddef>
#include <exception>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <type_traits>

#include "ErrorReporter.hpp"
#include "SourcePosition.hpp"
#include "Token.hpp"

Lexer::Lexer(std::string_view source, ErrorReporter &error_reporter)
    : m_source(source), m_error_reporter(error_reporter) {}

[[nodiscard]] auto Lexer::isAtEnd() const -> bool {
  return m_cur_cursor >= m_source.size();
}

void Lexer::nextLine() {
  ++m_current_line;
  m_current_line_position = 0;
}

void Lexer::moveCursor(std::uint32_t i) {
  m_cur_cursor += i;
  m_current_line_position += i;
}

auto Lexer::scanTokens() -> std::optional<
    std::reference_wrapper<std::vector<std::unique_ptr<Token> > > > {
  try {
    while (!isAtEnd() && !m_error_reporter.hasErrors()) {
      scanToken();
      m_prev_cursor = m_cur_cursor;

      m_start_line = m_current_line;
      m_start_line_position = m_current_line_position;
    }
    return m_tokens;
  } catch (std::exception &e) {
    m_error_reporter.setError("Unexpected error while lexing");
    return std::nullopt;
  }
}

void Lexer::scanToken()  // NOLINT(readability-function-cognitive-complexity)
{
  if (match('(')) {
    addToken<TokenType::TOKEN_LEFT_PAREN>();
  } else if (match(')')) {
    addToken<TokenType::TOKEN_RIGHT_PAREN>();
  } else if (match('{')) {
    addToken<TokenType::TOKEN_LEFT_BRACE>();
  } else if (match('}')) {
    addToken<TokenType::TOKEN_RIGHT_BRACE>();
  } else if (match(',')) {
    addToken<TokenType::TOKEN_COMMA>();
  } else if (match('.')) {
    addToken<TokenType::TOKEN_DOT>();
  } else if (match('-')) {
    addToken<TokenType::TOKEN_MINUS>();
  } else if (match('+')) {
    addToken<TokenType::TOKEN_PLUS>();
  } else if (match(';')) {
    addToken<TokenType::TOKEN_SEMICOLON>();
  } else if (match('*')) {
    addToken<TokenType::TOKEN_STAR>();
  } else if (match('?')) {
    addToken<TokenType::TOKEN_QUESTION>();
  } else if (match(':')) {
    addToken<TokenType::TOKEN_COLON>();
  } else if (match('!')) {
    if (match('=')) {
      addToken<TokenType::TOKEN_BANG_EQUAL>();
    } else {
      addToken<TokenType::TOKEN_BANG>();
    }
  } else if (match('=')) {
    if (match('=')) {
      addToken<TokenType::TOKEN_EQUAL_EQUAL>();
    } else {
      addToken<TokenType::TOKEN_EQUAL>();
    }
  } else if (match('<')) {
    if (match('=')) {
      addToken<TokenType::TOKEN_LESS_EQUAL>();
    } else {
      addToken<TokenType::TOKEN_LESS>();
    }
  } else if (match('>')) {
    if (match('=')) {
      addToken<TokenType::TOKEN_GREATER_EQUAL>();
    } else {
      addToken<TokenType::TOKEN_GREATER>();
    }
  } else if (match('/')) {
    if (match('*')) {
      while (!isAtEnd() && (peek() != '*' || peekNext() != '/')) {
        if (match('\n')) {
          nextLine();
        } else {
          moveCursor(1);
        }
      }
      if (!isAtEnd()) {
        moveCursor(2);
      }
    } else if (match('/')) {
      while (!isAtEnd() && peek() != '\n') {
        moveCursor(1);
      }
    } else {
      addToken<TokenType::TOKEN_SLASH>();
    }
  } else if (match('"')) {
    string();
  } else if (std::isdigit(peek()) != 0) {
    number();
  } else if (peek() == '_' || std::isalpha(peek()) != 0) {
    identifier();
  } else if (match('\n')) {
    nextLine();
  } else if (std::isspace(peek()) != 0) {
    moveCursor(1);
  } else {
    error("Unexpected character");
  }
}

[[nodiscard]] auto Lexer::match(char c) -> bool {
  if (peek() == c) {
    moveCursor(1);
    return true;
  }
  return false;
}

[[nodiscard]] auto Lexer::peek() const -> char {
  if (isAtEnd()) {
    return '\0';
  }
  return m_source.at(m_cur_cursor);
}

[[nodiscard]] auto Lexer::peekNext() const -> char {
  if (m_cur_cursor + 1 >= m_source.size()) {
    return '\0';
  }
  return m_source.at(m_cur_cursor + 1);
}

void Lexer::string() {
  while (!isAtEnd()) {
    if (match('"')) {
      auto string =
          m_source.substr(m_prev_cursor + 1, m_cur_cursor - m_prev_cursor - 2);
      addToken<TokenType::TOKEN_STRING>(string);
      return;
    }

    if (match('\n')) {
      nextLine();
    } else {
      moveCursor(1);
    }
  }

  error("Unterminated string");
}

void Lexer::number() {
  while (!isAtEnd() && std::isdigit(m_source.at(m_cur_cursor)) != 0) {
    moveCursor(1);
  }

  if ((std::isdigit(peekNext()) != 0) && match('.')) {
    while (!isAtEnd() && std::isdigit(m_source.at(m_cur_cursor)) != 0) {
      moveCursor(1);
    }
  }

  double value = 0;
  const auto res = std::from_chars(m_source.data() + m_prev_cursor,
                                   m_source.data() + m_cur_cursor, value);
  if (res.ec == std::errc()) {
    addToken<TokenType::TOKEN_NUMBER>(value);
  } else if (res.ec == std::errc::invalid_argument) {
    error("Invalid number conversion");
  } else if (res.ec == std::errc::result_out_of_range) {
    error("Out of range number conversion");
  }
}

void Lexer::identifier() {
  for (auto next = peek(); std::isalnum(next) != 0 || next == '_';
       next = peek()) {
    moveCursor(1);
  }

  auto lexeme = m_source.substr(m_prev_cursor, m_cur_cursor - m_prev_cursor);

  if (lexeme == "and") {
    addToken<TokenType::TOKEN_AND>();
  } else if (lexeme == "class") {
    addToken<TokenType::TOKEN_CLASS>();
  } else if (lexeme == "else") {
    addToken<TokenType::TOKEN_ELSE>();
  } else if (lexeme == "false") {
    addToken<TokenType::TOKEN_FALSE>();
  } else if (lexeme == "fun") {
    addToken<TokenType::TOKEN_FUN>();
  } else if (lexeme == "for") {
    addToken<TokenType::TOKEN_FOR>();
  } else if (lexeme == "if") {
    addToken<TokenType::TOKEN_IF>();
  } else if (lexeme == "nil") {
    addToken<TokenType::TOKEN_NIL>();
  } else if (lexeme == "or") {
    addToken<TokenType::TOKEN_OR>();
  } else if (lexeme == "print") {
    addToken<TokenType::TOKEN_PRINT>();
  } else if (lexeme == "return") {
    addToken<TokenType::TOKEN_RETURN>();
  } else if (lexeme == "super") {
    addToken<TokenType::TOKEN_SUPER>();
  } else if (lexeme == "this") {
    addToken<TokenType::TOKEN_THIS>();
  } else if (lexeme == "true") {
    addToken<TokenType::TOKEN_TRUE>();
  } else if (lexeme == "var") {
    addToken<TokenType::TOKEN_VAR>();
  } else if (lexeme == "while") {
    addToken<TokenType::TOKEN_WHILE>();
  } else {
    addToken<TokenType::TOKEN_IDENTIFIER>(lexeme);
  }
}

void Lexer::error(std::string const &error_msg) {
  m_error_reporter.setError(
      error_msg,
      SourcePosition(m_start_line, m_current_line, m_start_line_position,
                     m_current_line_position));
}