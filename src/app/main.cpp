#include <fcntl.h>
#include <sys/mman.h>
#include <sysexits.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <span>

#include "../../include/config.hpp"
#include "../lib/Lox.hpp"
#include "ReadLine.hpp"

auto runFile(std::string_view path) -> int {
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

  Lox lox;
  auto result = lox.run(source);
  std::uint16_t exit_code = 0;

  if (lox.hasErrors()) {
    for (auto const &error : lox.getErrors()) {
      std::cerr << error << std::endl;
    }
    exit_code = EX_SOFTWARE;
  } else if (result.has_value()) {
    for (auto const &v : result.value()) {
      std::cout << v << std::endl;
    }
  }

  munmap(map, file_size);
  close(fd);

  return exit_code;
}

auto runRepl() -> int {
  std::cout << "cpplox: Lox interpreter - v" << PROJECT_VER << std::endl;
  std::cout << "To exit, press Ctrl+d or type \"exit\"" << std::endl;

  Lox lox;
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

    auto result = lox.run(line.value());

    if (lox.hasErrors()) {
      for (auto const &error : lox.getErrors()) {
        std::cerr << error << std::endl;
      }
    } else if (result.has_value()) {
      for (auto const &v : result.value()) {
        std::cout << v << std::endl;
      }
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
