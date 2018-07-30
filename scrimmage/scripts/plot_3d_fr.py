#!/usr/bin/python
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
#
# This script generates a 3D plot of the trajectories of all of the entities
# as recorded from a run of SCRIMMAGE. 
#
# The trajectory data can be read from the frames.bin file that SCRIMMAGE 
# stores in a timestamped directory located in your `$HOME/swarm-log/` directory.
#
# To plot the most-recently modified frames.bin, run the following from
# the `/path/to/scrimmage/scripts` directory: 
#
#     $ python2.7 plot_3d_fr.py
#
# To plot a specific frames.bin, include that directory's path or the 
# frames.bin's path as an argument with the `-f` flag, e.g.:
#
#     $ python2.7 plot_3d_fr.py -f '$HOME/swarm-log/2016-09-22_10-37-38/'
#
#     or
#
#     $ python2.7 plot_3d_fr.py -f '$HOME/swarm-log/2016-09-22_10-37-38/frames.bin'
#

from __future__ import division, print_function
import argparse
import os
import glob
import time
import shutil
import struct
import scrimmage as sc

## imports for mplot3d
import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt
from itertools import cycle
from math import ceil

def find_frames(framesLoc=''):
    # Take argument of path to location of desired frames.bin 
    # (either containing directory or frames.bin itself)
    # If no argument provided, most recently-generated results 
    # directory is checked for frames.bin.
    # Return directory containing the frames.bin that will be
    # read and plotted.

    if not framesLoc:
        # if no path was provided (framesLoc = ''), look for the most recent run's results

        print('Location of results not specified.\n')

        # assuming default location of swarm-logs in user's home directory
        log_path = os.environ['HOME'] + '/swarm-log/*'
        # read back the results
        resultsDirsList = glob.glob(log_path)

        if not resultsDirsList:
            raise Exception('Error getting directories in ' + log_path + '\n')
        
        timeStampedDir = max(resultsDirsList, key=os.path.getctime)
        finalFramesDir = os.path.join(log_path, timeStampedDir) 
        print('Looking for frames.bin in most recent results directory:\n' + finalFramesDir)

    else:
        framesLocation = framesLoc 
        if framesLocation.startswith('~/'):
            framesLocation = framesLocation.replace('~', os.environ['HOME'])

        print('Looking for frames.bin in the following location:\n' + framesLocation)

        if os.path.exists(framesLocation):
            if not os.path.isfile(framesLocation): 
                # if specified location exists and is not a file, it's a directory
                finalFramesDir = framesLocation
            else:
                # specified location is a file
                if framesLocation.endswith('frames.bin'):
                    finalFramesDir = framesLocation[:-11]
                else:
                    # specified file was not frames.bin
                    raise Exception('Specified location was not frames.bin or its containing directory!\n')
        else:
            raise Exception('Specified location does not exist!\n')

    # Ensure finalFramesDir has trailing /
    if not finalFramesDir.endswith('/'):
        finalFramesDir = finalFramesDir + '/'
    # Is there a frames.bin file in this directory?
    if not os.path.isfile(finalFramesDir + 'frames.bin'):
        raise Exception('No frames.bin in ' + finalFramesDir + '\n')

    return finalFramesDir

def main():

    ## Command-line argument parsing ##
    parser=argparse.ArgumentParser(description='Plots trajectories of all entities after SCRIMMAGE simulation')
    parser.add_argument(type=str,
                        dest='frames_file',
                        default='',
                        help='path of frames.bin or its containing directory;\nif not specified, most recent frames.bin will be processed')

    args = parser.parse_args()

    ## find and read in the frames.bin for the case we want (or most recent case) ##
    finalFramesDir = find_frames(args.frames_file)
    finalFramesLoc = finalFramesDir + '/frames.bin'

    # Read in the frames from the protocol buffer
    print('Reading frames...\n')
    frames = sc.read_frames(finalFramesLoc)

    ## Organize data for plotting ##
    # Make a list of lists for each coordinate (one list per entity)
    allentX = [[] for ent in frames[0].contact]
    allentY = [[] for ent in frames[0].contact]
    allentZ = [[] for ent in frames[0].contact]

    for fr in frames: # at each timestep
        for con in fr.contact: # for each contact/entity
            conidx = con.id.id - 1
            # append each entity's x, y, z position to the appropriate list
            allentX[conidx].append(con.state.position.x)
            allentY[conidx].append(con.state.position.y)
            allentZ[conidx].append(con.state.position.z)

    ## Plot data ##
    # Preparing 3D trajectory plot
    trajFig = plt.figure(figsize=(18,12))
    ax = trajFig.gca(projection='3d')
    ax.set_xlabel('x position (m)')
    ax.set_ylabel('y position (m)')
    ax.set_zlabel('z position (m)')
    ax.set_title('Aircraft Trajectories')
  
    # Markers the plot can use
    mkCycler = cycle(mpl.markers.MarkerStyle.filled_markers)
    # How many markers on the longest-lived entity's trajectory line 
    num_mk = 8
    if num_mk > len(allentX):
        num_mk = len(allentX)
    # How many data points before a marker is placed
    mkInterval = ceil(len(frames)/num_mk)
    # Marker fillstyles (set by team)
    fs = ['full','none']
    #fs = mpl.markers.MarkerStyle.fillstyles # more fillstyles

    try:
        ## Plot each entity's trajectory and label with team number and agent number ##
        for con in frames[0].contact:
            conidx = con.id.id - 1
            labelstr = 'T' + str(con.id.team_id) + ', #' + str(conidx+1)
            
            ax.plot(allentX[conidx],allentY[conidx],allentZ[conidx],marker=next(mkCycler),markevery=mkInterval,fillstyle=fs[con.id.team_id-1],label=labelstr)
    except:
        raise Exception('Error during plotting procedure! Please verify that you are plotting a valid case.\n')

    # Shrink plot's axes to make some room for legend
    axLoc = ax.get_position()
    ax.set_position([axLoc.x0, axLoc.y0, axLoc.width*0.8, axLoc.height])

    # Create legend
    numEntPerCol = 30
    numLegCols = int(len(allentX)/numEntPerCol)
    if numLegCols < 1:
        numLegCols = 1
    leg = ax.legend(loc='center left',fontsize=9,markerscale=0.8,ncol=numLegCols,bbox_to_anchor=(1,0.5))
    leg.draggable(True)

    try:
        ## Show and save figure ##
        trajFig.show()
        trajFig.savefig(os.environ['SCRIMMAGE_ROOT']+'/trajectory_plot.png')

    except:
        raise Exception('Could not show plot. Please verify that you are plotting a valid case.') 

    # Press Enter to close the plot and end the program
    raw_input('Press Enter to close plot and exit.')

if __name__ == '__main__':
    main()



