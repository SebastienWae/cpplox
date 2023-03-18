#include "ErrorReporter.hpp"

#include <fmt/color.h>
#include <fmt/core.h>

#include <algorithm>
#include <any>
#include <range/v3/algorithm.hpp>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/view.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <string_view>
#include <utility>

Error::Error(std::string msg, std::optional<SourcePosition> position)
    : m_msg(std::move(msg)), m_position(std::move(position)) {}

ErrorReporter::ErrorReporter(Formater formater)
    : m_formater(std::move(formater)) {}

[[nodiscard]] auto ErrorReporter::hasErrors() const -> bool {
  return !m_errors.empty();
}

[[nodiscard]] auto ErrorReporter::getErrors() const
    -> std::vector<std::string> {
  auto s = m_source.at(0);
  auto i = defaultFormater(m_errors.at(0), m_source);
  return m_errors | ranges::views::transform([this](Error const& error) {
           return m_formater(error, m_source);
         }) |
         ranges::to_vector;
}

void ErrorReporter::setError(std::string const& msg,
                             std::optional<SourcePosition> position) {
  m_errors.emplace_back(msg, position);
}

void ErrorReporter::clearErrors() { m_errors.clear(); }

[[nodiscard]] auto ErrorReporter::defaultFormater(Error const& error,
                                                  std::string_view source)
    -> std::string {
  auto error_string =
      fmt::format("{0} {1}\n", fmt::styled("error:", fmt::fg(fmt::color::red)),
                  error.getMessage());

  auto error_position_opt = error.getPositon();
  if (error_position_opt.has_value()) {
    auto error_position = error_position_opt.value();
    auto error_source = error_position.getSourceSubstr(source);
    auto line_numbers_witdh = error_position.getEndLine() / 10 + 1;

    std::uint32_t line_index = 0;
    std::string_view::size_type prev_nl_position = 0;
    std::string_view::size_type next_nl_positon =
        error_source.find_first_of('\n');

    while (true) {
      auto line_source = error_source.substr(
          prev_nl_position, next_nl_positon - prev_nl_position);

      std::uint32_t start_err = 0;
      std::uint32_t end_err = line_source.size();
      if (line_index == 0) {
        start_err = error_position.getStartLinePosition();
      }
      if (line_index ==
          error_position.getEndLine() - error_position.getStartLine()) {
        end_err = error_position.getEndLinePosition();
      } else {
        end_err = end_err == 0 ? end_err : end_err - 1;
      }

      auto str_number_maring =
          fmt::format("{1:<{0}}", line_numbers_witdh,
                      error_position.getStartLine() + line_index);

      error_string += fmt::format(
          "{line_number_margin} | {line_source}\n{whitespace: <{padding_size}}"
          " | "
          "{error_indicator:>{error_indicator_position}}{error_underline:~>{"
          "error_underline_length}}\n",
          fmt::arg("line_number_margin", str_number_maring),
          fmt::arg("line_source", line_source), fmt::arg("whitespace", ""),
          fmt::arg("padding_size", str_number_maring.size()),
          fmt::arg("error_indicator",
                   fmt::styled("^", fmt::fg(fmt::color::red))),
          fmt::arg("error_indicator_position", start_err + 1),
          fmt::arg("error_underline",
                   fmt::styled("", fmt::fg(fmt::color::red))),
          fmt::arg("error_underline_length", end_err - start_err));

      if (next_nl_positon == std::string_view::npos) {
        break;
      }

      prev_nl_position = next_nl_positon + 1;
      next_nl_positon = error_source.find_first_of('\n', prev_nl_position);
      ++line_index;
    }
  }

  return error_string;
}

void ErrorReporter::setSource(std::string_view source) { m_source = source; }