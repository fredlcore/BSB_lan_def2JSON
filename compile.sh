#!/bin/sh
/usr/bin/clang++ --std=c++11 src/main.cpp -D'LANG=DE' -o main && ./main > output.json
rm main