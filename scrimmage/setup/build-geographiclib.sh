#!/bin/bash

source funcs.sh

THIRD_PARTY_BUILD_DIR=$(mkdir_check "$THIRD_PARTY_BUILD_DIR")

pushd $THIRD_PARTY_BUILD_DIR 

if [ ! -d geographiclib-code ]; then
    git clone git://git.code.sf.net/p/geographiclib/code geographiclib-code
fi    

pushd ./geographiclib-code >& /dev/null

mkdir -p ./build 

pushd ./build >& /dev/null

cmake ..
make_or_install "$1"

popd >& /dev/null # leave build
popd >& /dev/null # leave git repo
popd >& /dev/null # leave third-party-build
