#include <Token.hpp>

Token::Token (TokenType type, std::uint32_t offset, std::uint32_t length,
              std::uint32_t line)
    : m_type (type), m_offset (offset), m_length (length), m_line (line)
{
}