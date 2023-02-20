#include <ErrorReporter.hpp>
#include <fmt/format.h>

ErrorReporter::ErrorReporter(std::shared_ptr<std::ostream> stream_ptr)
    : m_stream_ptr(std::move(stream_ptr)) {}

[[nodiscard]] auto ErrorReporter::hasError() const -> bool {
  return !m_errors.empty();
}

void ErrorReporter::logErrors() const {
  for (auto const &error : m_errors) {
    *m_stream_ptr << error << std::endl;
  }
}

void ErrorReporter::setError(std::string const &error, std::uint32_t offset,
                             std::uint32_t length, std::uint32_t start_line,
                             std::uint32_t end_line) {
  m_errors.emplace_back(fmt::format(
      "{} - offset: {} - length: {} - start line: {} - end line: {}", error,
      offset, length, start_line, end_line));
}

void ErrorReporter::clearErrors() { m_errors.clear(); }