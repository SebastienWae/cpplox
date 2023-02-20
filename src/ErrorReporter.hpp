#ifndef CPPLOX_ERRORREPORTER_HPP
#define CPPLOX_ERRORREPORTER_HPP

#include <memory>
#include <string>
#include <vector>

class ErrorReporter {
  std::vector<std::string> m_errors;
  std::shared_ptr<std::ostream> m_stream_ptr;

public:
  ErrorReporter(std::shared_ptr<std::ostream> stream_ptr);

  [[nodiscard]] auto hasError() const -> bool;
  void logErrors() const;

  void setError(std::string const &error, std::uint32_t offset,
                std::uint32_t length, std::uint32_t start_line,
                std::uint32_t end_line);
  void clearErrors();
};

#endif /* CPPLOX_ERRORREPORTER_HPP */
