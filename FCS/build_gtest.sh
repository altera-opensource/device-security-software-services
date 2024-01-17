#!/bin/bash
if [ -f "./gtest/lib/libgtest_main.a" ]; then
    echo "Skipped building gtest: already present."
    exit 0
fi
mkdir gtest && cd gtest
git clone https://github.com/google/googletest.git -b release-1.11.0
cd googletest
mkdir build
cd build
cmake ..
make
cd ../..
cp -R ./googletest/build/lib ./
mkdir include
cp -R ./googletest/googletest/include/gtest ./include/gtest
cp -R ./googletest/googlemock/include/gmock/ ./include/gtest
rm -rf ./googletest
