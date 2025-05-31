from ubuntu:latest

RUN apt-get update --fix-missing && \
    apt-get install -y --no-install-recommends \
    cmake \
    make \
    gcc-14 \
    g++-14 \
    libsfml-dev \
    git \
    pipx && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100

RUN pipx install clang-format && \
    pipx ensurepath

