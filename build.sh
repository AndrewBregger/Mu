#!/usr/bin/sh

# if the directory does not exist then create it
[ ! -f "build" ] || mkdir build

pushd build
cmake ..
popd

make -C build/ -j4
