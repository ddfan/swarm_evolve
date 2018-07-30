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

if [ "$#" -lt 3 ]; then
    echo "usage: $0 <PluginType> <PluginName> <PluginsDirectory>"
    exit 2
fi

PLUGIN_TYPE="$1"
PLUGIN_NAME="$2"
PLUGINS_DIR="$(readlink -f $3)"

if [ $PLUGIN_TYPE != "autonomy" ] && [ $PLUGIN_TYPE != "motion" ]  && [ $PLUGIN_TYPE != "interaction" ] ; then
    echo "Invalid plugin type."
    echo "PluginType must be \"autonomy\", \"motion\", or \"interaction\""
    exit 2
fi

if [ ! -d "$PLUGINS_DIR" ];
then
    echo "PluginsDirectory doesn't exist."
    exit 1
fi

# Jump to the directory that holds this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
pushd ${DIR} >& /dev/null

###############################################################################
# Use the plugin template to generate the plugin
###############################################################################
rm -rf "$PLUGIN_NAME"  # clear out any plugin with this name that may exist
cp -r ./plugin-template/$PLUGIN_TYPE "$PLUGIN_NAME"

H_FILE_IN=$(readlink -f ./$PLUGIN_NAME/PluginTemplate.h)
CPP_FILE_IN=$(readlink -f ./$PLUGIN_NAME/PluginTemplate.cpp)
XML_FILE_IN=$(readlink -f ./$PLUGIN_NAME/PluginTemplate.xml)

H_FILE_OUT=$(readlink -f ./$PLUGIN_NAME/$PLUGIN_NAME.h)
CPP_FILE_OUT=$(readlink -f ./$PLUGIN_NAME/$PLUGIN_NAME.cpp)
XML_FILE_OUT=$(readlink -f ./$PLUGIN_NAME/$PLUGIN_NAME.xml)
CMAKE_FILE_OUT=$(readlink -f ./$PLUGIN_NAME/CMakeLists.txt)

mv $H_FILE_IN $H_FILE_OUT
mv $CPP_FILE_IN $CPP_FILE_OUT
mv $XML_FILE_IN $XML_FILE_OUT

sed -i -- "s/(>>>PLUGIN_NAME<<<)/$PLUGIN_NAME/g" $H_FILE_OUT
sed -i -- "s/(>>>PLUGIN_NAME<<<)/$PLUGIN_NAME/g" $CPP_FILE_OUT
sed -i -- "s/(>>>PLUGIN_NAME<<<)/$PLUGIN_NAME/g" $XML_FILE_OUT
sed -i -- "s/(>>>PLUGIN_NAME<<<)/$PLUGIN_NAME/g" $CMAKE_FILE_OUT

###############################################################################
# Move the generated plugin to the plugin directory
###############################################################################
rm -rf "$PLUGINS_DIR/$PLUGIN_NAME" # clear out any plugin with this name that
                                   # may exist
mv "$PLUGIN_NAME" "$PLUGINS_DIR"

pushd >& /dev/null

################################################################################
## Modify the CMakeList.txt file in the plugin directory
################################################################################
# This is no longer needed. There is a CMake script that GLOBS the
#subdirectories of the plugins directory.  

#CMAKELISTS_FILE=$(readlink -f "$PLUGINS_DIR/CMakeLists.txt")
#
#if [ ! -e $CMAKE_FILE ];
#then
#    echo "Can't edit plugins CMakeLists.txt file, it doesn't exist."
#    exit -1
#fi
#
## We need to add the "add_subdirectory(MyPlugin)" line to the CMakeLists.txt
## file. First, we need to see if the line already exists. If it does exist,
## exit successfully, if not, we need to add the line.
#LINE_VALUE="add_subdirectory(${PLUGIN_NAME})"
#
## See if the CMakeLists.txt file already contains the line:
#grep -i -q $LINE_VALUE $CMAKELISTS_FILE
#LINE_EXISTS=$?
#
## grep returns 0 if the line exists.
#if [ $LINE_EXISTS -eq 0 ];
#then
#    # We don't have to edit file
#    exit 0
#fi
#
## The line doesn't exist yet, append it to the CMakeLists.txt file
#echo $LINE_VALUE >> $CMAKELISTS_FILE
