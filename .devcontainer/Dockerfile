# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.234.0/containers/debian/.devcontainer/base.Dockerfile

# [Choice] Debian version (use bullseye on local arm64/Apple Silicon): bullseye, buster
ARG VARIANT="buster"
FROM mcr.microsoft.com/vscode/devcontainers/base:0-${VARIANT}

# ** [Optional] Uncomment this section to install additional packages. **
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>

# Add bullseye repository and install qemu-system-misc from bullseye
RUN echo "deb http://deb.debian.org/debian bullseye main contrib non-free" > /etc/apt/sources.list.d/bullseye.list \
    && apt-get update \
    && apt-get install -y -t bullseye qemu-system-misc

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu 

# Create directory and symbolic link for stdarg.h
RUN mkdir -p /usr/lib/gcc-cross/riscv64-linux-gnu/8/include \
    && ln -s /usr/lib/gcc-cross/riscv64-linux-gnu/10/include/stdarg.h /usr/lib/gcc-cross/riscv64-linux-gnu/8/include/stdarg.h

# enable loading custom .gdbinit per folder.
# RUN echo set auto-load safe-path / > /root/.gdbinit
RUN echo set architecture riscv:rv64 > /root/.gdbinit