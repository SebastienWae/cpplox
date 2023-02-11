FROM ubuntu:22.04

RUN set -eux \
  && apt-get update \
  && apt-get install -y git cmake gdb rr valgrind g++-12 clang-15 clang-13 \
    libc++-15-dev libstdc++-10-dev clangd-15 clang-format-15 clang-tidy-15 \
    cppcheck iwyu \
  && apt-get clean

CMD ["/bin/bash"]
