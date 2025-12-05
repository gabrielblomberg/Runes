FROM ubuntu:latest

RUN apt-get update --fix-missing && \
    apt-get install -y --no-install-recommends \
    sudo \
    git \
    wget \
    cmake \
    make \
    ccache \
    gdb \
    gcc-14 \
    g++-14 \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libudev-dev \
    libflac-dev \
    libvorbis-dev \
    libgl1-mesa-dev \
    libegl1-mesa-dev \
    libfreetype-dev \
    pipx && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100

RUN cd /tmp \
    && mkdir -p sfml \
    && cd sfml \
    && wget https://www.sfml-dev.org/files/SFML-3.0.0-linux-gcc-64-bit.tar.gz \
    && tar zxf SFML-3.0.0-linux-gcc-64-bit.tar.gz \
    && cp -r SFML-3.0.0/include/* /usr/include \
    && cp -r SFML-3.0.0/lib/* /usr/lib/ \
    && cp -r SFML-3.0.0/share/* /usr/share/ \
    && ldconfig \
    && cd .. \
    && rm -rf sfml

RUN (echo "deb http://apt.llvm.org/noble/ llvm-toolchain-noble-21 main"; \
     echo "deb-src http://apt.llvm.org/noble/ llvm-toolchain-noble-21 main") > /etc/apt/sources.list.d/llvm.list && \
    wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key > /etc/apt/trusted.gpg.d/apt.llvm.org.asc && \
    apt-get update --fix-missing && \
    apt-get install -y --no-install-recommends \
      clangd-21 \
      clang-tidy-21 \
      clang-format-21 && \
    ln -s /usr/bin/clang-21 /usr/local/bin/clang && \
    ln -s /usr/bin/clang++-21 /usr/local/bin/clang++ && \
    ln -s /usr/bin/clangd-21 /usr/local/bin/clangd && \
    ln -s /usr/bin/clang-tidy-21 /usr/local/bin/clang-tidy && \
    ln -s /usr/bin/clang-format-21 /usr/local/bin/clang-format

RUN useradd -m -s /bin/bash dev && echo 'dev ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER dev

ENTRYPOINT [ "sh", "-c", "trap 'exit 0' TERM; sleep infinity & wait" ]

