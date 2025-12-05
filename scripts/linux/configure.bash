#!/bin/bash
cmake \
    -DCMAKE_C_COMPILER=gcc-14 \
    -DCMAKE_CXX_COMPILER=g++-14 \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_BUILD_TYPE=$1 \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -S src \
    -B build
