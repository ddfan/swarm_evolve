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
#include "SimpleAircraftControllerPID.h"
#include <boost/algorithm/string.hpp>
#include <iostream>

REGISTER_PLUGIN(scrimmage::Controller,
                SimpleAircraftControllerPID,
                SimpleAircraftControllerPID_plugin)

namespace sc = scrimmage; 

void set_pid(sc::PID &pid, std::string str, bool is_angle) {
    std::vector<std::string> str_vals;
    boost::split(str_vals, str, boost::is_any_of(","));

    if (str_vals.size() != 4) {
        std::cout << "error parsing in SimpleAircraftControllerPID" << std::endl;
    } else {
        double p = std::stod(str_vals[0]);
        double i = std::stod(str_vals[1]);
        double d = std::stod(str_vals[2]);
        pid.set_parameters(p, i, d);

        if (is_angle) {
            double i_lim = sc::Angles::deg2rad(std::stod(str_vals[3]));
            pid.set_integral_band(i_lim);
            pid.set_is_angle(true);
        } else {
            double i_lim = std::stod(str_vals[3]);
            pid.set_integral_band(i_lim);
        }
    }
}

void SimpleAircraftControllerPID::init(std::map<std::string, std::string> &params) {
    set_pid(heading_pid_, params["heading_pid"], true);
    set_pid(alt_pid_, params["alt_pid"], false);
    set_pid(vel_pid_, params["vel_pid"], false);
    u_ = std::make_shared<Eigen::Vector3d>();
}

bool SimpleAircraftControllerPID::step(double t, double dt) {
    double desired_yaw = desired_state_->quat().yaw();                    
     
    heading_pid_.set_setpoint(desired_yaw);
    double u_heading = heading_pid_.step(dt, state_->quat().yaw());     
    double roll_error = u_heading + state_->quat().roll();               
     
    alt_pid_.set_setpoint(desired_state_->pos()(2));
    double u_alt = alt_pid_.step(dt, state_->pos()(2));
    double pitch_error = (-u_alt - state_->quat().pitch());     
        
    vel_pid_.set_setpoint(desired_state_->vel()(0));
    double u_thrust = vel_pid_.step(dt, state_->vel().norm());     
    
    (*u_) << u_thrust, roll_error, pitch_error;
    return true;
}
