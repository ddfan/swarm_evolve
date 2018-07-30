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
#include "JSBSimControlControllerHeadingPID.h"
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/math/State.h>
#include <boost/algorithm/string.hpp>

REGISTER_PLUGIN(scrimmage::Controller,
                JSBSimControlControllerHeadingPID,
                JSBSimControlControllerHeadingPID_plugin)

namespace sc = scrimmage; 
using ang = scrimmage::Angles;

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

void JSBSimControlControllerHeadingPID::init(std::map<std::string, std::string> &params) {
    angles_from_jsbsim_.set_input_clock_direction(ang::Rotate::CW);
    angles_from_jsbsim_.set_input_zero_axis(ang::HeadingZero::Pos_Y);
    angles_from_jsbsim_.set_output_clock_direction(ang::Rotate::CCW);
    angles_from_jsbsim_.set_output_zero_axis(ang::HeadingZero::Pos_X);

    angles_to_jsbsim_.set_input_clock_direction(ang::Rotate::CCW);
    angles_to_jsbsim_.set_input_zero_axis(ang::HeadingZero::Pos_X);
    angles_to_jsbsim_.set_output_clock_direction(ang::Rotate::CW);
    angles_to_jsbsim_.set_output_zero_axis(ang::HeadingZero::Pos_Y);

    set_pid(roll_pid_, params["roll_pid"], true);
    set_pid(pitch_pid_, params["pitch_pid"], true);
    set_pid(yaw_pid_, params["yaw_id"], true);
}

bool JSBSimControlControllerHeadingPID::step(double t, double dt) {

    // Roll stabilizer
    roll_pid_.set_setpoint(0);
    double u_roll = roll_pid_.step(dt, state_->quat().roll());
    u_(0) = ang::angle_pi(u_roll);

    // Pitch stabilizer
    pitch_pid_.set_setpoint(0);
    u_(1) = -pitch_pid_.step(dt, state_->quat().pitch());        
    
    // Yaw stabilizer
    double desired_yaw = desired_state_->quat().yaw();
    angles_to_jsbsim_.set_angle(ang::rad2deg(desired_yaw));    
    yaw_pid_.set_setpoint(ang::deg2rad(angles_to_jsbsim_.angle()));
    u_(2) = -yaw_pid_.step(dt, state_->quat().yaw());    

    return true;
}

