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

if [ "$#" -ne 1 ]; then
    echo "usage: $0 <jsbsim-code root directory>"
    exit -1;
fi

JSBSIM_ROOT_DIR=$(readlink -f $1)

# Jump to the script's directory
BASEDIR=$(dirname "$0")
pushd $BASEDIR >& /dev/null

# Get absolute paths to the patch files
PATCH_1=$(readlink -f ./patches/jsbsim.patch)

pushd $JSBSIM_ROOT_DIR >& /dev/null

# Apply patches
git apply $PATCH_1

popd >& /dev/null # return to scripts directory
popd >& /dev/null # return to original calling directory
