#include <fcntl.h>
#include <sys/mman.h>
#include <sysexits.h>
#include <unistd.h>

#include <iostream>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>

#include "../include/config.hpp"
#include "../lib/ErrorReporter.hpp"
#include "../lib/Expression.hpp"
#include "../lib/Interpreter.hpp"
#include "../lib/Lexer.hpp"
#include "../lib/Parser.hpp"
#include "../lib/Token.hpp"
#include "ReadLine.hpp"

void run(std::string_view source, ErrorReporter &error_reporter) {
  Lexer lexer{source, error_reporter};
  auto tokens = lexer.scanTokens();

  if (!error_reporter.hasError() && tokens.has_value()) {
    Parser parser{tokens.value(), error_reporter};
    auto statements = parser.parse();

    if (!error_reporter.hasError() && statements.has_value()) {
      Interpreter interpreter{statements.value(), error_reporter};
      interpreter.interpret();
    }
  }
}

auto runFile(std::string_view path) -> int {
  std::shared_ptr<std::ostream> stream_ptr(&std::cerr, [](std::ostream *) {});
  ErrorReporter error_reporter(stream_ptr);

  auto fd = open(path.data(), O_RDONLY);
  if (fd == -1) {
    std::cerr << "Failed to open file " << path << '\n';
    return EX_NOINPUT;
  }

  auto file_size = lseek(fd, 0, SEEK_END);
  auto *map = mmap(nullptr, file_size, PROT_READ, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) {
    std::cerr << "Failed to map file " << path << '\n';
    close(fd);
    return EX_NOINPUT;
  }

  std::string_view source(reinterpret_cast<const char *>(map), file_size);

  run(source, error_reporter);

  munmap(map, file_size);
  close(fd);

  if (error_reporter.hasError()) {
    error_reporter.logErrors();
    return EX_SOFTWARE;
  }

  return 0;
}

auto runRepl() -> int {
  std::shared_ptr<std::ostream> stream_ptr(&std::cerr, [](std::ostream *) {});
  ErrorReporter error_reporter(stream_ptr);

  std::cout << "cpplox: Lox interpreter - v" << PROJECT_VER << std::endl;
  std::cout << "To exit, press Ctrl+d or type \"exit\"" << std::endl;

  ReadLine readline{">> "};
  while (true) {
    auto line = readline.getLine();

    if (!line.has_value()) {
      std::cout << "\n";
      return 0;
    }

    if (line->empty()) {
      continue;
    }

    if (line == "exit") {
      return 0;
    }

    run(line.value(), error_reporter);

    if (error_reporter.hasError()) {
      error_reporter.logErrors();
      error_reporter.clearErrors();
    }
  }
}

auto main(int argc, char **argv) -> int {
  std::span const args(argv, argc);

  if (argc > 2) {
    std::cerr << "Usage: " << args.front() << " [filename]" << std::endl;
    return EX_USAGE;
  }
  if (argc == 2) {
    return runFile(args.back());
  }
  return runRepl();
}
