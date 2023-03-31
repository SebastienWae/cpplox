FROM ubuntu:22.04

# ------------------------- install required packages ------------------------ #
RUN set -eux \
  && apt-get update \
  && DEBIAN_FRONTEND=noninteractive apt-get install -y git cmake cmake-format ninja-build gdb rr valgrind g++-12 clang-15 clang-13 \
    libc++-15-dev libstdc++-10-dev clangd-15 clang-format-15 clang-tidy-15 \
    cppcheck iwyu pkg-config ccache curl zip unzip \
  && apt-get clean

# --------------------------- create non root user --------------------------- #
ARG USERNAME=dev
ARG USER_UID=1000
ARG USER_GID=$USER_UID
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && chsh -s /usr/bin/bash $USERNAME

USER $USERNAME