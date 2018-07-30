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
#include "SimpleQuadrotorControllerLQR.h"
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>
#include <boost/algorithm/clamp.hpp>

REGISTER_PLUGIN(scrimmage::Controller,
                SimpleQuadrotorControllerLQR,
                SimpleQuadrotorControllerLQR_plugin)

namespace sc = scrimmage; 

void SimpleQuadrotorControllerLQR::init(std::map<std::string, std::string> &params) {
    double p_gain = sc::get("vel_p_gain", params, 1.0);
    double i_gain = sc::get("vel_i_gain", params, 1.0);
    double d_gain = sc::get("vel_d_gain", params, 1.0);
    double i_lim = sc::get("i_lim", params, 1.0);
    vel_pid_.set_parameters(p_gain, i_gain, d_gain);
    vel_pid_.set_integral_band(i_lim);

    max_vel_ = std::stod(params["max_vel"]);
}

bool SimpleQuadrotorControllerLQR::step(double t, double dt) {
    Eigen::Vector3d &des_pos = desired_state_->pos();
    double des_yaw = desired_state_->quat().yaw();

    Eigen::Vector3d &pos = state_->pos();
    Eigen::Vector3d &vel = state_->vel();
    double yaw = state_->quat().yaw();
    double xy_speed = vel.head<2>().norm();

    double yaw_dot =
        std::isnan(prev_yaw_) ? 0 : sc::Angles::angle_diff_rad(yaw, prev_yaw_) / dt;
    prev_yaw_ = yaw;

    // LQR Altitude Controller:
    double q1 = 1;
    double z_thrust = -1.0 / q1 * (pos(2) - des_pos(2)) - sqrt(2.0/q1) * vel(2);
     
    // LQR Heading Controller:
    double q2 = 1;
    double turn_force = -1.0 / q2 * sc::Angles::angle_pi(yaw - des_yaw) - sqrt(2.0/q2) * yaw_dot;        
    
    // If not close to x/y position, use forward velocity:
    double dist = (des_pos - pos).head<2>().norm();
    vel_pid_.set_setpoint((dist < 10) ? 0 : dist / 10);    

    double ctrl_vel = vel_pid_.step(dt, xy_speed);

    u_(0) = boost::algorithm::clamp(ctrl_vel, -max_vel_, max_vel_);
    u_(1) = 0;
    u_(2) = z_thrust;
    u_(3) = turn_force;
     
    return true;
}
