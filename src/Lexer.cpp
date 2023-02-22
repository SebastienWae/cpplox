#include "Token.hpp"
#include <Lexer.hpp>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <exception>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <type_traits>

Lexer::Lexer (std::string_view source, ErrorReporter &error_reporter)
    : m_source (source), m_error_reporter (error_reporter)
{
}

[[nodiscard]] auto
Lexer::isAtEnd () const -> bool
{
  return m_pos_current >= m_source.size ();
}

auto
Lexer::scanTokens () -> std::optional<
    std::reference_wrapper<std::vector<std::unique_ptr<Token> > > >
{
  try
    {
      while (!isAtEnd () && !m_error_reporter.hasError ())
        {
          scanToken ();
          m_pos_start = m_pos_current;
          m_line_start = m_line_current;
        }
      return m_tokens;
    }
  catch (std::exception &e)
    {
      return std::nullopt;
    }
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void
Lexer::scanToken ()
{
  auto char_at_point = m_source.at (m_pos_current++);

  switch (char_at_point)
    {
    case '(':
      addToken<TokenType::TOKEN_LEFT_PAREN> ();
      break;
    case ')':
      addToken<TokenType::TOKEN_RIGHT_PAREN> ();
      break;
    case '{':
      addToken<TokenType::TOKEN_LEFT_BRACE> ();
      break;
    case '}':
      addToken<TokenType::TOKEN_RIGHT_BRACE> ();
      break;
    case ',':
      addToken<TokenType::TOKEN_COMMA> ();
      break;
    case '.':
      addToken<TokenType::TOKEN_DOT> ();
      break;
    case '-':
      addToken<TokenType::TOKEN_MINUS> ();
      break;
    case '+':
      addToken<TokenType::TOKEN_PLUS> ();
      break;
    case ';':
      addToken<TokenType::TOKEN_SEMICOLON> ();
      break;
    case '*':
      addToken<TokenType::TOKEN_STAR> ();
      break;
    case '!':
      match ('=') ? addToken<TokenType::TOKEN_BANG_EQUAL> ()
                  : addToken<TokenType::TOKEN_BANG> ();
      break;
    case '=':
      match ('=') ? addToken<TokenType::TOKEN_EQUAL_EQUAL> ()
                  : addToken<TokenType::TOKEN_EQUAL> ();
      break;
    case '<':
      match ('=') ? addToken<TokenType::TOKEN_LESS_EQUAL> ()
                  : addToken<TokenType::TOKEN_LESS> ();
      break;
    case '>':
      match ('=') ? addToken<TokenType::TOKEN_GREATER_EQUAL> ()
                  : addToken<TokenType::TOKEN_GREATER> ();
      break;
    case '/':
      {
        if (match ('*'))
          {
            while (!isAtEnd () && !(match ('*') && match ('/')))
              {
                if (peek () == '\n')
                  {
                    ++m_line_current;
                  }
                ++m_pos_current;
              }
          }
        else if (match ('/'))
          {
            while (peek () != '\n' && !isAtEnd ())
              {
                ++m_pos_current;
              }
          }
        else
          {
            addToken<TokenType::TOKEN_SLASH> ();
          }
        break;
      }
    case '"':
      string ();
      break;
    default:
      {
        if (std::isdigit (char_at_point) != 0)
          {
            number ();
          }
        else if (char_at_point == '_' || (std::isalpha (char_at_point) != 0))
          {
            identifier ();
          }
        else if (char_at_point == '\n')
          {
            ++m_line_current;
          }
        else if (std::isspace (char_at_point) != 0)
          {
            return;
          }
        else
          {
            m_error_reporter.setError ("Unexpected character", m_pos_start,
                                       m_pos_current - m_pos_start,
                                       m_line_start, m_line_current);
          }
      }
    }
}

auto
Lexer::match (char const &expected) -> bool
{
  if (isAtEnd () || m_source.at (m_pos_current) != expected)
    {
      return false;
    }

  ++m_pos_current;
  return true;
}

[[nodiscard]] auto
Lexer::peek () const -> char
{
  if (isAtEnd ())
    {
      return '\0';
    }
  return m_source.at (m_pos_current);
}

[[nodiscard]] auto
Lexer::peekNext () const -> char
{
  if (m_pos_current + 1 >= m_source.size ())
    {
      return '\0';
    }
  return m_source.at (m_pos_current + 1);
}

void
Lexer::string ()
{
  while (peek () != '"' && !isAtEnd ())
    {
      if (peek () == '\n')
        {
          ++m_line_current;
        }
      ++m_pos_current;
    }

  if (isAtEnd ())
    {
      m_error_reporter.setError ("Unterminated string", m_pos_start,
                                 m_pos_current - m_pos_start, m_line_start,
                                 m_line_current);
      return;
    }

  auto string
      = m_source.substr (m_pos_start + 1, m_pos_current - m_pos_start - 1);
  addToken<TokenType::TOKEN_STRING> (string);
  ++m_pos_current;
}

void
Lexer::number ()
{
  while (!isAtEnd () && std::isdigit (m_source.at (m_pos_current)) != 0)
    {
      ++m_pos_current;
    }

  if (peek () == '.' && (std::isdigit (peekNext ()) != 0))
    {
      ++m_pos_current;

      while (!isAtEnd () && std::isdigit (m_source.at (m_pos_current)) != 0)
        {
          ++m_pos_current;
        }
    }

  double value = 0;
  const auto res = std::from_chars (m_source.data () + m_pos_start,
                                    m_source.data () + m_pos_current, value);
  if (res.ec == std::errc ())
    {
      addToken<TokenType::TOKEN_NUMBER> (value);
    }
  else if (res.ec == std::errc::invalid_argument)
    {
      m_error_reporter.setError ("Invalid number conversion", m_pos_start,
                                 m_pos_current - m_pos_start, m_line_start,
                                 m_line_current);
    }
  else if (res.ec == std::errc::result_out_of_range)
    {
      m_error_reporter.setError ("Out of range number conversion", m_pos_start,
                                 m_pos_current - m_pos_start, m_line_start,
                                 m_line_current);
    }
}

void
Lexer::identifier ()
{
  for (auto next = peek (); std::isalnum (next) != 0 || next == '_';
       next = peek ())
    {
      ++m_pos_current;
    }

  auto lexeme = m_source.substr (m_pos_start, m_pos_current - m_pos_start);

  if (lexeme == "and")
    {
      addToken<TokenType::TOKEN_AND> ();
    }
  else if (lexeme == "class")
    {
      addToken<TokenType::TOKEN_CLASS> ();
    }
  else if (lexeme == "else")
    {
      addToken<TokenType::TOKEN_ELSE> ();
    }
  else if (lexeme == "false")
    {
      addToken<TokenType::TOKEN_FALSE> ();
    }
  else if (lexeme == "fun")
    {
      addToken<TokenType::TOKEN_FUN> ();
    }
  else if (lexeme == "for")
    {
      addToken<TokenType::TOKEN_FOR> ();
    }
  else if (lexeme == "if")
    {
      addToken<TokenType::TOKEN_IF> ();
    }
  else if (lexeme == "nil")
    {
      addToken<TokenType::TOKEN_NIL> ();
    }
  else if (lexeme == "or")
    {
      addToken<TokenType::TOKEN_OR> ();
    }
  else if (lexeme == "print")
    {
      addToken<TokenType::TOKEN_PRINT> ();
    }
  else if (lexeme == "return")
    {
      addToken<TokenType::TOKEN_RETURN> ();
    }
  else if (lexeme == "super")
    {
      addToken<TokenType::TOKEN_SUPER> ();
    }
  else if (lexeme == "this")
    {
      addToken<TokenType::TOKEN_THIS> ();
    }
  else if (lexeme == "true")
    {
      addToken<TokenType::TOKEN_TRUE> ();
    }
  else if (lexeme == "var")
    {
      addToken<TokenType::TOKEN_VAR> ();
    }
  else if (lexeme == "while")
    {
      addToken<TokenType::TOKEN_WHILE> ();
    }
  else
    {
      addToken<TokenType::TOKEN_IDENTIFIER> (lexeme);
    }
}