#ifndef CPPLOX_ERRORREPORTER_HPP
#define CPPLOX_ERRORREPORTER_HPP

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "SourcePosition.hpp"

class Error {
  std::string const m_msg;
  std::optional<SourcePosition> m_position;

 public:
  Error(std::string msg, std::optional<SourcePosition> position);

  [[nodiscard]] inline auto getMessage() const -> std::string const& {
    return m_msg;
  }
  [[nodiscard]] inline auto getPositon() const
      -> std::optional<SourcePosition> {
    return m_position;
  }
  [[nodiscard]] inline auto hasPosition() const {
    return m_position.has_value();
  }
};

class ErrorReporter {
 public:
  using Formater = std::function<std::string(Error, std::string_view)>;

 private:
  std::vector<Error> m_errors;
  std::string_view m_source;
  Formater m_formater;

 public:
  ErrorReporter(Formater formater = defaultFormater);

  void setSource(std::string_view source);

  [[nodiscard]] auto hasErrors() const -> bool;
  [[nodiscard]] auto getErrors() const -> std::vector<std::string>;

  void setError(std::string const& msg,
                std::optional<SourcePosition> position = std::nullopt);

  void clearErrors();

 private:
  [[nodiscard]] static auto defaultFormater(Error const& error,
                                            std::string_view source)
      -> std::string;
};

#endif /* CPPLOX_ERRORREPORTER_HPP */
