SCRIMMAGE Simulator
================

Install Dependencies
----------------------

A list of the Ubuntu packages required is provided in
./setup/install-binaries.sh in the "DEPS_DPKG" array. Run our automated
installer to install the required packages:

    $ sudo ./setup/install-binaries.sh

Build SCRIMMAGE
------------------------

Scrimmage requires geographiclib, jsbsim, protobuf, and grpc to be built from
source. You can build these libraries externally, or you can use our built-in
system to download and build the libraries. The libraries will not be installed
to your local system. They will reside in the ./build/3rd-party directory, but
our cmake system can find them.

    $ mkdir build
    $ cd build
    $ cmake ..
    $ source setenv.sh
    $ make

By default, python bindings are built for python 2.7.
If you want to build for a different python version, pass a `PYTHON_VERSION` flag to `cmake`, e.g. replace `cmake ..` with

    $ cmake .. -DPYTHON_VERSION=3.5

Install protobuf Python package
---------------------------
Protobuf should be installed using the source files that are compiled during build.
Using protobuf from PyPI (i.e. what you get with `pip install protobuf`) is known to cause crashes.

    $ cd /path/to/scrimmage/build/3rd-party/src/protobuf/python/
    $ python setup.py install

Build GRPC Python Bindings
---------------------------

    $ cd /path/to/scrimmage/build/3rd-party/src/grpc
    $ pip install -rrequirements.txt                # python 2.7 bindings
    $ pip3 install -rrequirements.txt               # python 3 bindings
    $ GRPC_PYTHON_BUILD_WITH_CYTHON=1 pip install .      # python 2.7 bindings
    $ GRPC_PYTHON_BUILD_WITH_CYTHON=1 pip3 install .     # python 3 bindings

Make sure the Python futures package wasn't install in your python3
distribution.

    $ sudo pip3 uninstall futures


Install Scrimmage Python Bindings
------------------------
To install python bindings, first make sure the GRPC python bindings are installed, then run

    $ pip install /path/to/scrimmage/python/

Replace `pip` with `pip3` if you have built python 3 bindings.
Note that if you are in a virtual environment, `pip` (without the 3) might be the installer for python 3.

Testing SCRIMMAGE
--------------------

Open a new terminal, change to the scrimmage directory, and execute a mission.

    $ cd scrimmage
    $ scrimmage ./missions/straight.xml

You should see the visualization GUI open up and display the simulation.

GUI Commands
------------------
The GUI responds to the following input keys:

    'b'                : (Break) Pauses and unpauses the simulation.
    'space bar'        : When paused, take a single simulation step.
    'a'                : Rotate through the camera views
    'right/left arrows : Change the aircraft to follow
    '['                : Decrease simulation warp speed
    ']'                : Increase simulation warp speed
    '+'                : Increase object size
    '-'                : Decrease object size

The GUI's camera can operate in three modes (cycle with 'a' key):
1. Follow the entity and point towards the entity's heading
2. Free floating camera
3. Follow the entity from a fixed viewpoint

Note: There is a known bug in VTK (the visualization GUI that we use) that will
result in the GUI crashing if the user hits the 'p' key. We are working to
resolve this.

Note: If all of the terrain data does not appear, click on the GUI window with
your mouse. This is a VTK bug.

Cleaning SCRIMMAGE
--------------------------------------
The scrimmage source code can be cleaned with the standard clean command:

    $ make clean

However, you can do a full clean (including downloaded and built 3rd-party
source code):

    $ make distclean

Build your own autonomy plugin
---------------------------------------

  The generate-plugin.sh script will create a C++ plugin for you. You just need
  to supply the type of plugin, the name of the plugin, and the directory to
  install the plugin source code. For example, if you want to create an
  autonomy plugin called "MyAutonomyPlugin" and install it into scrimmage's
  own autonomy plugins directory, execute the following commands:

    $ cd /path/to/scrimmage
    $ ./scripts/generate-plugin.sh autonomy MyAutonomyPlugin ./plugins/autonomy


Obtain Pre-Built SCRIMMAGE Docker Image
-----------------------------------------------

The SCRIMMAGE docker image isn't publicly available yet, but for GTRI
employees, they can use the internal docker registry.

1. Allow non-https connections for your docker client. Add the following
   information to your /etc/docker/daemon.json file:
   
        {
            "dns": ["10.104.30.150", "8.8.8.8"],
            "insecure-registries" : ["10.108.21.229:5000"]
        }

    Reference:
    https://docs.docker.com/registry/insecure/

2. Restart docker service:

        $ sudo service docker restart
        
3. Pull the image:

        $ docker pull 10.108.21.229:5000/scrimmage:latest
        
4. Run scrimmage (without GUI):

        $ docker run -it 10.108.21.229:5000/scrimmage:latest /bin/bash
        $ scrimmage ./missions/straight-no-gui.xml


Installing and Configuring Open Grid Engine
-----------------------------------------------

Instructions modified from:
    https://scidom.wordpress.com/2012/01/18/sge-on-single-pc/
    http://www.bu.edu/tech/support/research/system-usage/running-jobs/tracking-jobs/

Install Grid Engine:

    $ sudo apt-get install gridengine-master gridengine-exec gridengine-common gridengine-qmon gridengine-client

Note that you can configure how qsub is called with a `.sge_request` in your
home directory. Further, you can set the number of available slots (cores
available) when running grid engine under the Queue Control tab.

Installing, Configuring, and Using Docker
-----------------------------------------------
1. Setup an openssh server

    sudo apt install openssh-server

2. Generate keys somewhere (i.e., override the default path to the ssh key)

    ssh-keygen

3. Add the new key to the authorized keys

    cat /path/to/new/id_rsa.pub >> ~/.ssh/authorized_keys
    sudo service ssh restart

4. Get your ip address

    nmap -p 22 localhost

5. Build docker images

    cd /path/to/scrimmage/ci
    ./build.sh ./ubuntu16.04
    ./build.sh ./ubuntu14.04

6. You can then step into the image with

    docker run -i -t scrimmage:16.04 /bin/bash

7. Docker troubleshooting:
Docker can have DNS issues. If you can ping a public ip address within a docker
image (such as 8.8.8.8), but you can't ping archive.ubuntu.com, create the file
/etc/docker/daemon.json with the following contents:

    {
        "dns": ["<DNS-IP>", "8.8.8.8"]
    }

Where <DNS-IP> is the first DNS IP address from the command:

    nmcli dev list | grep 'IP4.DNS'          # Ubuntu <= 14
    nmcli device show <interfacename> | grep IP4.DNS   # Ubuntu >= 15

Restart docker:

    sudo service docker restart



Troubleshooting
-------------------------------------------

#### Problem: I cannot load python libraries through scrimmage ####

Make sure that when you run the cmake command it is using the version of python
that you want to use with the following:

    $ cmake -DPYTHON_EXECUTABLE:FILEPATH=/usr/bin/python      \  # adjust path to your needs
            -DPYTHON_INCLUDE_DIR:PATH=/usr/include/python2.7  \  # adjust path to your needs
            -DPYTHON_LIBRARY:FILEPATH=/usr/lib/libpython2.7.so   # adjust path to your needs

#### Problem: Error `virtual memory exhausted: Cannot allocate memory` during `make` step of installation. ####

Cause: Your system has run out of RAM and out of swap (virtual memory) to complete the installation.

#### Possible Solution Steps: ####

1. Add more RAM to your system (or allocate more RAM to your VM, if applicable).
2. Increase the size of your system's swap partition.

#### Problem: No entities show up when I run swarmsim. ####

Likely cause: This seems to happen most often when Ubuntu is being run in a VM
(virtual machine) using a platform such as VirtualBox.

#### Possible Solution Steps: ####

1. Ensure that `scrimmage/build/setenv.sh` has been sourced in the terminal
from which you are running SCRIMMAGE:

        $ cd /path/to/scrimmage
        $ source ./build/setenv.sh

2. Reboot your machine.
3. Ensure that your video card drivers are up to date (AMD Radeon 16.7.3 works
with VirtualBox as of 9/6/2016).
4. Adjust your VM's settings so 3D acceleration is enabled. For example, in
VirtualBox, this setting can be found by clicking your VM in VirtualBox Manager,
then go to Settings\>Display\>Screen\>Acceleration and ensure the box labeled
"Enable 3D Acceleration" is checked.

#### Possible Solutions to Miscellaneous Problems: ####

1. Ensure that `scrimmage/build/setenv.sh` has been sourced in the terminal
from which you are running SCRIMMAGE:

        $ cd /path/to/scrimmage
        $ source ./build/setenv.sh

2. Ensure all dependencies have been properly installed by (re-)running:

        $ sudo path/to/scrimmage/setup/install-binaries.sh



#### Common Error Messages ####

1. Error Message:
   [libprotobuf ERROR google/protobuf/descriptor_database.cc:57] File already
   exists in database: AuctionMsgs.proto

   Solution ::

        $ cd ~/scrimmage/scrimmage/build
        $ make clean
        $ rm ../msg_libs
        $ rm ../plugin_libs
        $ make
