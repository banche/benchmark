#!/bin/bash

mkdir -p build
pushd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_TESTING=OFF -DFMT_INSTALL=ON ../
popd
