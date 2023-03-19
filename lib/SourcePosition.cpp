#include "SourcePosition.hpp"

#include <string_view>

SourcePosition::SourcePosition(std::uint32_t start_line, std::uint32_t end_line,
                               std::uint32_t start_line_postion,
                               std::uint32_t end_line_position)
    : m_start_line(start_line),
      m_end_line(end_line),
      m_start_line_postion(start_line_postion),
      m_end_line_position(end_line_position) {}

[[nodiscard]] auto SourcePosition::getSourceSubstr(
    std::string_view source) const -> std::string_view {
  std::string_view::size_type line_start = 0;
  auto line_length = std::string_view::npos;

  std::uint32_t line_index = 1;
  std::string_view::size_type cursor_pos = 0;

  for (; cursor_pos < source.length(); ++cursor_pos) {
    if (line_index == m_start_line) {
      line_start = cursor_pos;
      break;
    }
    if (source.at(cursor_pos) == '\n') {
      ++line_index;
    }
  }
  for (; cursor_pos < source.length(); ++cursor_pos) {
    if (line_index == m_end_line) {
      line_length = source.find_first_of('\n', cursor_pos);
      if (line_length > 0 && line_length != std::string_view::npos) {
        line_length -= line_start;
      }
      break;
    }
    if (source.at(cursor_pos) == '\n') {
      ++line_index;
    }
  }

  return source.substr(line_start, line_length);
}