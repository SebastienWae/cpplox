#include "ReadLine.hpp"
#include <cstdlib>
#include <optional>
#include <readline/history.h>
#include <readline/readline.h>
#include <string_view>

ReadLine::ReadLine (std::string_view const prompt) : m_prompt (prompt) {}

ReadLine::~ReadLine ()
{
  freeLine ();
  rl_clear_history ();
}

[[nodiscard]] auto
ReadLine::getLine () -> std::optional<std::string_view>
{
  freeLine ();

  auto *line = readline (m_prompt.data ());

  if (line != nullptr)
    {
      if (*line != 0)
        {
          add_history (line);
        }
      m_line = line;
    }

  return m_line;
}

void
ReadLine::freeLine ()
{
  if (m_line.has_value ())
    {
      free (m_line.value ());
      m_line = std::nullopt;
    }
}