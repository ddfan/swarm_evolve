Notes and Tips for working with FlightGear
===========================================


Run FlightGear in "Slave" Mode
-------------------------------

1. Make sure that aircraft.xml file has the following output enabled:
   <output name="localhost" type="FLIGHTGEAR" port="5600" rate="60" protocol="UDP"/>

2. Launch Flight Gear:

   $ fgfs --airport=KSFO --runway=28R --aircraft=c172p --native-fdm=socket,out,60,,5500,udp --fdm=null --native-fdm=socket,in,60,,5600,udp

3. Launch JSBSim (real-time)

   $ cd /path/to/jsbsim-code
   $ JSBSim --realtime ./scripts/c172_takeoff.xml

