#ifndef CPPLOX_SOURCEPOSITION_HPP
#define CPPLOX_SOURCEPOSITION_HPP

#include <cstdint>
#include <string_view>

class SourcePosition {
  std::uint32_t const m_start_line;
  std::uint32_t const m_end_line;
  std::uint32_t const m_start_line_postion;
  std::uint32_t const m_end_line_position;

 public:
  SourcePosition(std::uint32_t start_line, std::uint32_t end_line,
                 std::uint32_t start_line_postion,
                 std::uint32_t end_line_position);

  [[nodiscard]] inline auto getStartLine() const { return m_start_line; }
  [[nodiscard]] inline auto getEndLine() const { return m_end_line; }
  [[nodiscard]] inline auto getStartLinePosition() const {
    return m_start_line_postion;
  }
  [[nodiscard]] inline auto getEndLinePosition() const {
    return m_end_line_position;
  }
  [[nodiscard]] auto getSourceSubstr(std::string_view source) const
      -> std::string_view;
};

#endif /* CPPLOX_SOURCEPOSITION_HPP */
