#!/bin/sh

set -e

if [ "$ARCH" = x86_64 ]; then
  bits=64
else
  bits=32
fi

# Use this mingw instead of the pre-installed mingw on Appveyor
if [ "$COMPILER" = gcc ]; then
    f=mingw-w$bits-bin-$ARCH-20200211.7z
    if ! [ -e $f ]; then
	echo "Downloading $f"
	curl -LsSO https://sourceforge.net/projects/mingw-w64-dgn/files/mingw-w64/$f
    fi
    7z x $f > /dev/null
    export PATH=$PWD/mingw$bits/bin:$PATH
    export CC=$PWD/mingw$bits/bin/gcc
fi

# Build
cd /c/projects/hts-engine-api/src
mkdir -p build && cd build
# NOTE: it seems this does not work as expected...
if [ "$COMPILER" = gcc ]; then
  cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..
else
  cmake ..
fi
cmake --build . --config Release