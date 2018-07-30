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

usage()
{
    cat << EOF

USAGE: 
          $0 [OPTIONS]

OPTIONS:
          -h   Display this usage information
          -t   Sets the number of simulation runs (tasks)
          -m   Mission file
          -r   Ranges file for generating new scenarios
          -p   Maximum number of parallel runs (tasks)

DEFAULTS:
          -t=2   # 2 simulation runs
          -p=1   # 1 simulation at-a-time

EXAMPLE:
          Run 10 simulations, 4 at-a-time, straight.xml mission, 
          with test-1.xml ranges file:
              $0 -t 10 -p 4 -m ~/scrimmage/scrimmage/missions/straight.xml \\
              -r ~/scrimmage/scrimmage/config/ranges/test-1.xml
EOF
}

TASKS=2
MAX_PARALLEL_TASKS=1
MISSION_FILE="UNDEFINED"
RANGES_FILE="UNDEFINED"
while getopts ":t:m:r:p:h" opt; do
    case $opt in
        h)
            usage
            exit 0
            ;;
        t)
            TASKS=$OPTARG
            ;;
        m)
            MISSION_FILE=$OPTARG
            ;;
        r)
            RANGES_FILE=$OPTARG
            ;;
        p)
            MAX_PARALLEL_TASKS=$OPTARG
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            usage
            exit 1
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            usage
            exit 1
            ;;
    esac
done

if [ ! -e $MISSION_FILE ]; then
    echo "Mission file doesn't exist: $MISSION_FILE"
    usage
    exit -1
fi

# see if the user is using the ranges file
RANGES_OPTION=""
if [ -e $RANGES_FILE ]; then
    RANGES_OPTION="--ranges=$(readlink -f $RANGES_FILE)"
fi

if [ $MAX_PARALLEL_TASKS -gt $(nproc) ]; then
    echo "===================================================================="
    echo "WARNING: MAX_PARALLEL_TASKS is set to $MAX_PARALLEL_TASKS,"
    echo "but you only have $(nproc) processors."
    echo "===================================================================="
fi

MISSION_FILE=$(readlink -f $MISSION_FILE)

ROOT_LOG=$(readlink -f "${HOME}/swarm-log")
LOG_FILE_DIR=${ROOT_LOG}/log
MISSION_FILE_DIR=${ROOT_LOG}/missions

mkdir -p $LOG_FILE_DIR
mkdir -p $ROOT_LOG

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
pushd $SCRIPT_DIR >& /dev/null

python generate_scenarios.py \
    --num_runs=$TASKS ${RANGES_OPTION} \
    --only_xml \
    $MISSION_FILE \
    $ROOT_LOG

GENERATE_STATUS=$?
if [ ! $GENERATE_STATUS -eq 0 ]; then
    echo "Failed to generate scenarios"
    usage
    exit -1
fi

# Load up an array with mission IDs and mission files.
IDS_FILES=()
for i in `seq 1 ${TASKS}`;
do
    MISSION=$(readlink -f "${MISSION_FILE_DIR}/${i}.xml")
    IDS_FILES=("${IDS_FILES[@]}" "$i" "${MISSION}")
done

parallel --bar -j $MAX_PARALLEL_TASKS -n 2 \
    "scrimmage -j 0 -t {1} {2} > ${LOG_FILE_DIR}/{1}.log 2>&1" \
    ::: ${IDS_FILES[@]}

echo "Completed ${TASKS} simulations."

popd >& /dev/null
