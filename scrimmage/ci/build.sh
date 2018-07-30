#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo
    echo "Usage: $0 <dockerfile-dir>"
    echo
    exit -1;
fi

DOCKERFILE_DIR=$(readlink -f "$1")

# Jump to the directory that holds this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
pushd $DIR >& /dev/null

DOCKER_BASE_IMAGE=$(basename $DOCKERFILE_DIR)
cp id_rsa "$DOCKERFILE_DIR/id_rsa"
sudo docker build --build-arg USER=$USER -t scrimmage:${DOCKER_BASE_IMAGE} $DOCKERFILE_DIR 
rm "$DOCKERFILE_DIR/id_rsa"

popd >& /dev/null

# Run Image:
#sudo docker run -i -t <IMAGE ID>  /bin/bash
