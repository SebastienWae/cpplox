#ifndef CPPLOX_READLINE_HPP
#define CPPLOX_READLINE_HPP

#include <optional>
#include <string_view>

class ReadLine {
  std::string_view m_prompt;
  std::optional<char *> m_line = std::nullopt;

 public:
  ReadLine(std::string_view prompt);
  ReadLine(const ReadLine &) = delete;
  ReadLine(ReadLine &&) = delete;
  auto operator=(const ReadLine &) -> ReadLine & = default;
  auto operator=(ReadLine &&) -> ReadLine & = delete;
  ~ReadLine();

  [[nodiscard]] auto getLine() -> std::optional<std::string_view>;

 private:
  void freeLine();
};

#endif /* CPPLOX_READLINE_HPP */
