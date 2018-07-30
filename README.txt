Author: David Fan
Paper:  Model-Based Stochastic Search for Large Scale Optimization of Multi-Agent UAV Swarms 
Arxiv: https://arxiv.org/abs/1803.01106
Date: 7/30/18

Abstract:
Recent work from the reinforcement learning community has shown that Evolution Strategies are a fast and scalable alternative to other reinforcement learning methods. In this paper we show that Evolution Strategies are a special case of model-based stochastic search methods. This class of algorithms has nice asymptotic convergence properties and known convergence rates. We show how these methods can be used to solve both cooperative and competitive multi-agent problems in an efficient manner. We demonstrate the effectiveness of this approach on two complex multi-agent UAV swarm combat scenarios: where a team of fixed wing aircraft must attack a well-defended base, and where two teams of agents go head to head to defeat each other. 


Getting Started:

This is intended to help someone get started in running simulations on a swarm scenario and to help a person understand how the different components work together, in order to allow them to run their own custom simulations.  The main purpose of the project is to investigate the creation of swarm behaviors in a team scenario using a neural network controller and an evolution-strategies learning algorithm.  It should be possible to easily change the scenario in terms of the number of players, the scoring rubrick, the randomness of the initial placement of the planes or quadcopters, and much more.

Installation should proceed by installing both scrimmage and scrimmage-gtri-share.  Follow the READMEs in both folders.  The code has been modified from the open-source SCRIMMAGE project (https://www.scrimmagesim.org/).  The two main modifications are the addition of two programs: scrimmage-learn and scrimmage-learncompete.  scrimmage-learn allows one to learn a nn controller for a team of planes or other vehicles going up against an enemy team with hand-coded behavior.  Each plane has the same controller nn.  The input to the nn are the plane's states, as well as local friends and enemies' states, and base directions.  The outputs are the desired altitude, heading, and velocity.  These three outputs are fed to a PID controller which then determines the best low-level controls (roll, pitch, yaw, thrust) to achieve the desired heading.

After installing, to run the program from /scrimmage/bin, run:

./scrimmage-learn ../scrimmage-gtri-share/missions/capture-the-flag-learn.xml.

The xml file specifies all the details of the simulation, including duration, number of threads, learning parameters, what behaviors the vehicles should take, etc etc.

The behavior of the planes are specified by scrimmage-gtri-share/plugins/autonomy/CaptureTheFlagLearn/CaptureTheFlagLearn.cpp.  Some parameters for their behavior are found in the xml file in that folder.

Scoring is handled by scrimmage-gtri-share/plugins/metrics/CaptureTheFlagMetrics/CaptureTheFlagMetrics.xml for the scenario interactions, and by scrimmage/plugins/metrics/SimpleCollisionMetrics/SimpleCollisionMetrics.xml for the ground, team, and non-team collisions.

Tiny DNN (https://tiny-dnn.readthedocs.io) is used for the neural networks.  The header-only library is included in the include folder.  Also included is cereal (https://uscilab.github.io/cereal/).

After training a network one can replay the behavior using the program scrimmage-playlearned.  Same usage as above, specify the mission file.  Make sure the path to the desired learned neural network "nn.dat" is specified in the mission xml file.






