FROM ubuntu:latest

RUN apt-get update --fix-missing && \
    apt-get install -y --no-install-recommends \
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

# RUN pipx install clang-format && \
#     pipx ensurepath
#
# ARG USERNAME=vscode
# ARG USER_UID=1001
# ARG USER_GID=$USER_UID
#
# RUN groupadd --gid $USER_GID $USERNAME \
#     && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
#     && apt-get update \
#     && apt-get install -y sudo \
#     && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
#     && chmod 0440 /etc/sudoers.d/$USERNAME
#
# USER $USERNAME
