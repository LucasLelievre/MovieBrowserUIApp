#!/bin/bash
mkdir build
cd build
cmake ..
cmake --build . --config Release --clean-first
if [ $? -eq 0 ]
then
    echo "build successful"
    ./MovieBrowser
else
    echo "build failed" >&2
fi