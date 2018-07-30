#!/bin/bash

source funcs.sh

THIRD_PARTY_BUILD_DIR=$(mkdir_check "$THIRD_PARTY_BUILD_DIR")

pushd $THIRD_PARTY_BUILD_DIR 

if [ ! -d ./grpc* ]; then
    git clone -b $(curl -L http://grpc.io/release) https://github.com/grpc/grpc
fi

pushd ./grpc* >& /dev/null

git submodule update --init

make_or_install "$1"

popd >& /dev/null # leave build
popd >& /dev/null # leave repo
popd >& /dev/null # leave third-party-build
