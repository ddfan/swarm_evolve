#!/bin/bash
# ---------------------------------------------------------------------------
# @section LICENSE
#  
# Copyright (c) 2016 Georgia Tech Research Institute (GTRI) 
#               All Rights Reserved
#  
# The above copyright notice and this permission notice shall be included in 
# all copies or substantial portions of the Software.
#  
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
# DEALINGS IN THE SOFTWARE.
# ---------------------------------------------------------------------------
# @file filename.ext
# @author Kevin DeMarco <kevin.demarco@gtri.gatech.edu> 
# @author Eric Squires <eric.squires@gtri.gatech.edu>
# @version 1.0
# ---------------------------------------------------------------------------
# @brief A brief description.
# 
# @section DESCRIPTION
# A long description.
# ---------------------------------------------------------------------------

# Jump to the directory that holds this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
pushd $DIR >& /dev/null

source funcs.sh

# Require the script to be run as root
if [[ $(/usr/bin/id -u) -ne 0 ]]; then
    echo "This script must be run as root."
    exit -1
fi

# Create a 3rd-party-src directory
THIRD_PARTY_BUILD_DIR=$(readlink -f ./third-party-build)
runasuser "mkdir -p $THIRD_PARTY_BUILD_DIR"

###############################################################################
# Install binaries using the system's package manager
###############################################################################
./install-binaries.sh

# Build JSBSim
runasuser ./build-jsbsim.sh
#./build-jsbsim.sh install

# Build Geographic Lib
runasuser ./build-geographiclib.sh 
./build-jsbsim.sh install

## Build protocol buffers 3
#runasuser ./build-protobuf3.sh
#./build-protobuf3.sh install
#
## Build grpc
#runasuser ./build-grpc.sh
#./build-grpc.sh install

# Build SCRIMMAGE
./build-scrimmage.sh

# Add source setenv.sh to user's .bashrc file
SOURCE_LINE="source $(readlink -f ./setenv.sh)"
echo $SOURCE_LINE
grep "$SOURCE_LINE" "${HOME}/.bashrc" > /dev/null 2>&1
if [ $? != 0 ]; then
    runasuser "echo $SOURCE_LINE >> ${HOME}/.bashrc"
fi

popd >& /dev/null # return to original calling directory
