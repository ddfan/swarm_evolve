#!/bin/bash
#scrimmage.sh
#$ -V
#
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

MISSION_FILE_DIR="UNDEFINED"
while getopts ":d:" opt; do
    case $opt in        
        d)
            MISSION_FILE_DIR=$OPTARG
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            exit 1
            ;;
    esac
done

if [ ! -e $MISSION_FILE_DIR ]; then
    echo "Mission file directory doesn't exist: $MISSION_FILE"
    echo "usage: $0 -m <mission-file>"
    exit -1
fi

EXEC=$(readlink -f ${SCRIMMAGE_ROOT}/bin/scrimmage)

MISSION_FILE="$MISSION_FILE_DIR/$SGE_TASK_ID.xml"

# Run SCRIMMAGE
$EXEC -j $JOB_ID -t $SGE_TASK_ID $MISSION_FILE
