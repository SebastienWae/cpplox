FROM ubuntu:22.04

# ------------------------- install required packages ------------------------ #
RUN set -eux \
  && apt-get update \
  && apt-get install -y git cmake gdb rr valgrind g++-12 clang-15 clang-13 \
    libc++-15-dev libstdc++-10-dev clangd-15 clang-format-15 clang-tidy-15 \
    cppcheck iwyu \
  && apt-get clean

# --------------------------- create non root user --------------------------- #
ARG USERNAME=dev
ARG USER_UID=1000
ARG USER_GID=$USER_UID
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && chsh -s /usr/bin/bash $USERNAME

USER $USERNAME