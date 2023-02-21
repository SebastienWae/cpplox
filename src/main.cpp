#include "ErrorReporter.hpp"
#include "Lexer.hpp"
#include "Token.hpp"
#include <config.hpp>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <sys/mman.h>
#include <sysexits.h>
#include <unistd.h>

void run(std::string_view source, ErrorReporter &error_reporter) {
  Lexer lexer{source, error_reporter};
  auto tokens = lexer.scanTokens();
}
auto runFile(std::string_view path) -> int {
  std::shared_ptr<std::ostream> stream_ptr(&std::cerr, [](std::ostream *) {});
  ErrorReporter error_reporter(stream_ptr);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
  auto fd = open(path.data(), O_RDONLY);
  if (fd == -1) {
    std::cerr << "Failed to open file " << path << '\n';
    return EX_NOINPUT;
  }

  auto file_size = lseek(fd, 0, SEEK_END);
  auto *map = mmap(nullptr, file_size, PROT_READ, MAP_SHARED, fd, 0);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast,performance-no-int-to-ptr)
  if (map == MAP_FAILED) {
    std::cerr << "Failed to map file " << path << '\n';
    close(fd);
    return EX_NOINPUT;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
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

  while (true) {
    std::string source;

    std::cout << "> ";
    std::getline(std::cin, source);

    if (std::cin.bad()) {
      std::cerr << "Failed to read standard input" << std::endl;
      return EX_IOERR;
    }

    if (std::cin.eof()) {
      std::cout << std::endl;
      return 0;
    }

    if (source == "exit") {
      return 0;
    }

    run(source, error_reporter);

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
