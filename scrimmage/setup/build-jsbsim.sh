#!/bin/bash

source funcs.sh

PATCH_SCRIPT=$(readlink -f ./apply-jsbsim-patch.sh)
INSTALL_AIRCRAFT_SCRIPT=$(readlink -f ./install-aircraft.sh)

THIRD_PARTY_BUILD_DIR=$(mkdir_check "$THIRD_PARTY_BUILD_DIR")

pushd $THIRD_PARTY_BUILD_DIR 

if [ ! -d jsbsim-code ]; then
    git clone git://git.code.sf.net/p/jsbsim/code jsbsim-code
fi    

pushd ./jsbsim-code >& /dev/null

$PATCH_SCRIPT .
$INSTALL_AIRCRAFT_SCRIPT .

mkdir -p ./build 

pushd ./build >& /dev/null

cmake ..
make_or_install "$1"

popd >& /dev/null # leave build
popd >& /dev/null # leave jsbsim-code
popd >& /dev/null # leave third-party-build
