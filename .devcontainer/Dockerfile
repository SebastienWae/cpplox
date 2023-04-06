FROM ubuntu:23.04

# ------------------------- install required packages ------------------------ #
RUN set -eux \
  && apt-get update \
  && DEBIAN_FRONTEND=noninteractive apt-get install -y git cmake cmake-format \ 
    ninja-build gdb rr valgrind g++ clang libc++-dev clangd clang-format \ 
    clang-tidy cppcheck iwyu pkg-config ccache curl zip unzip \
  && apt-get clean

# --------------------------- create non root user --------------------------- #
ARG USERNAME=dev
ARG USER_UID=1001
ARG USER_GID=$USER_UID
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && chsh -s /usr/bin/bash $USERNAME

USER $USERNAME