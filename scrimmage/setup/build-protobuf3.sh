#!/bin/bash

source funcs.sh

THIRD_PARTY_BUILD_DIR=$(mkdir_check "$THIRD_PARTY_BUILD_DIR")

pushd $THIRD_PARTY_BUILD_DIR 

if [ ! -d ./protobuf* ]; then
    git clone https://github.com/google/protobuf.git
fi

pushd ./protobuf* >& /dev/null

# For some reason, you need to run autogen and configure two times in the
# protobuf repo to generate the Makefile. This is a hack, but also ensures that
# we don't keep running autogen after Makefile has been generated.
while [ ! -e "./Makefile" ];
do
    ./autogen.sh
    ./configure
done

make_or_install "$1"

popd >& /dev/null # leave build
popd >& /dev/null # leave repo
popd >& /dev/null # leave third-party-build
