#!/bin/bash

source funcs.sh

pushd ../ >& /dev/null

mkdir -p ./build 

pushd ./build >& /dev/null

cmake ..
make_or_install "$1"

popd >& /dev/null # leave build
popd >8 /dev/null # back to original directory
