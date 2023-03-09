#include "ErrorReporter.hpp"

#include <fmt/core.h>

ErrorReporter::ErrorReporter(std::shared_ptr<std::ostream> stream_ptr)
    : m_stream_ptr(std::move(stream_ptr)) {}

[[nodiscard]] auto ErrorReporter::hasError() const -> bool {
  return !m_errors.empty();
}

void ErrorReporter::logErrors() const {
  for (auto const &error : m_errors) {
    *m_stream_ptr << fmt::format("{}\n{}", error.first, error.second)
                  << std::endl;
  }
}

void ErrorReporter::setError(std::string const &from, std::string const &msg) {
  m_errors.emplace_back(from, msg);
}

void ErrorReporter::clearErrors() { m_errors.clear(); }