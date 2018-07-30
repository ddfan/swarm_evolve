/// ---------------------------------------------------------------------------
/// @section LICENSE
///  
/// Copyright (c) 2016 Georgia Tech Research Institute (GTRI) 
///               All Rights Reserved
///  
/// The above copyright notice and this permission notice shall be included in 
/// all copies or substantial portions of the Software.
///  
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
/// DEALINGS IN THE SOFTWARE.
/// ---------------------------------------------------------------------------
/// @file filename.ext
/// @author Kevin DeMarco <kevin.demarco@gtri.gatech.edu> 
/// @author Eric Squires <eric.squires@gtri.gatech.edu>
/// @version 1.0
/// ---------------------------------------------------------------------------
/// @brief A brief description.
/// 
/// @section DESCRIPTION
/// A long description.
/// ---------------------------------------------------------------------------
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include "RigidBody6DOFControllerROS.h"
#include <boost/algorithm/string.hpp>

REGISTER_PLUGIN(scrimmage::Controller,
                RigidBody6DOFControllerROS,
                RigidBody6DOFControllerROS_plugin)

namespace sc = scrimmage;

using std::cout;
using std::endl;

void RigidBody6DOFControllerROS::init(std::map<std::string, std::string> &params) {
    u_ = std::make_shared<Eigen::Vector4d>();
    
    if (!ros::isInitialized()) {
        int argc = 0;
        std::string name = "simple_aircraft_3d_controller";
        ros::init(argc, NULL, name);        
    }
    nh_ = std::make_shared<ros::NodeHandle>();
    cmd_vel_sub_ = nh_->subscribe("cmd_vel", 1, &RigidBody6DOFControllerROS::cmd_vel_cb, this);
}

bool RigidBody6DOFControllerROS::step(double t, double dt) {
    ros::spinOnce();    

    // Convert twist to aircraft control surfaces
    double thrust = 0, elevator = 0, aileron = 0, rudder = 0;

    thrust = cmd_vel_.linear.x;
    elevator = cmd_vel_.angular.y;
    aileron = cmd_vel_.angular.x;
    rudder = cmd_vel_.angular.z;
    
    (*u_) << thrust, elevator, aileron, rudder;
    //(*u_) << thrust, elevator, aileron;
       
    return true;
}

void RigidBody6DOFControllerROS::cmd_vel_cb(const geometry_msgs::Twist::ConstPtr& msg)
{
    cmd_vel_ = *msg;
}
