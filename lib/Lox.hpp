#ifndef CPPLOX_LOX_HPP
#define CPPLOX_LOX_HPP

#include <optional>
#include <string_view>
#include <vector>

#include "Environment.hpp"
#include "ErrorReporter.hpp"

class Lox {
  ErrorReporter m_error_reporter;
  Environment m_environment{m_error_reporter};

 public:
  auto run(std::string_view source) -> std::optional<std::string const>;

  [[nodiscard]] auto getErrors() const -> std::vector<std::string>;
  [[nodiscard]] auto hasErrors() const -> bool;
};

#endif /* CPPLOX_LOX_HPP */
