FROM ubuntu:eoan AS builder

ARG POL_SRC_DIR=/tmp/polserver
ARG POL_RELEASE_URL=https://github.com/polserver/polserver/archive/9b8fe4e33a24d99941145cfffaa0ba25aad852f5.zip
ARG POL_BUILD_TYPE=Release
ARG COMPILER_VERSION=8
ARG NINJA_VERSION=1.9.0
ARG CMAKE_VERSION=3.16.2

ENV CXX=g++-$COMPILER_VERSION
ENV CC=gcc-$COMPILER_VERSION
ENV CMAKE_ARCHIVE=cmake-$CMAKE_VERSION-Linux-x86_64
ENV NINJA_BIN_URL="https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip"
ENV CMAKE_BIN_URL="https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/${CMAKE_ARCHIVE}.tar.gz"

RUN cd /tmp \
 && apt-get update \
 && apt-get install $CXX libssl-dev wget unzip libz-dev -y \
 && apt-get clean \
 && rm -r /var/lib/apt/lists/* \
 && wget -q $POL_RELEASE_URL -O pol.zip \
 && unzip -q pol.zip \
 && cd polserver* \
 && wget -q $NINJA_BIN_URL \
 && unzip -q ninja-linux.zip -d bin-build \
 && wget -q $CMAKE_BIN_URL -O "cmake.tar.gz" \
 && tar -xf "cmake.tar.gz" \
 && export PATH="$(pwd)/bin-build:$(pwd)/$CMAKE_ARCHIVE/bin:${PATH}" \
 && chmod +x bin-build/ninja \
 && chmod +x "$CMAKE_ARCHIVE/bin/cmake" \
 && cd bin-build \
 && cmake .. -G Ninja -DNO_PCH=0 -DCMAKE_BUILD_TYPE=$POL_BUILD_TYPE \
 && cmake --build . --config $POL_BUILD_TYPE --target package \
 && mkdir -p /app/ \
 && mv ../bin/_CPack_Packages/Linux/ZIP/*/* /app/ \
 && cd /app \
 && rm -rf /tmp/*