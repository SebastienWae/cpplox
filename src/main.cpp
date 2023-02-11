#include <iostream>
#include <span>
#include <string_view>

void runFile(std::string_view const &path) {}
void runRepl() {}

auto main(int argc, char **argv) -> int {
  std::span const args(argv, argc);

  if (argc > 2) {
    std::cout << "Usage: " << args.front() << " [filename]" << std::endl;
    return 1;
  }
  if (argc == 2) {
    runFile(args.back());
  } else {
    runRepl();
  }
}