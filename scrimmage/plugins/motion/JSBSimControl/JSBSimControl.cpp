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
#include "JSBSimControl.h"

#include <GeographicLib/LocalCartesian.hpp>
#include <initialization/FGTrim.h>

#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/math/Angles.h>
#include <scrimmage/math/State.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>

#define meters2feet 3.28084
#define feet2meters (1.0 / meters2feet)

REGISTER_PLUGIN(scrimmage::MotionModel, JSBSimControl, JSBSimControl_plugin)

namespace sc = scrimmage;
using ang = scrimmage::Angles;

JSBSimControl::JSBSimControl()
{
    angles_from_jsbsim_.set_input_clock_direction(ang::Rotate::CW);
    angles_from_jsbsim_.set_input_zero_axis(ang::HeadingZero::Pos_Y);
    angles_from_jsbsim_.set_output_clock_direction(ang::Rotate::CCW);
    angles_from_jsbsim_.set_output_zero_axis(ang::HeadingZero::Pos_X);

    angles_to_jsbsim_.set_input_clock_direction(ang::Rotate::CCW);
    angles_to_jsbsim_.set_input_zero_axis(ang::HeadingZero::Pos_X);
    angles_to_jsbsim_.set_output_clock_direction(ang::Rotate::CW);
    angles_to_jsbsim_.set_output_zero_axis(ang::HeadingZero::Pos_Y);        
}

std::tuple<int,int,int> JSBSimControl::version()
{
    return std::tuple<int,int,int>(0,0,1);
}

bool JSBSimControl::init(std::map<std::string, std::string> &info,
                         std::map<std::string, std::string> &params)
{
    roll_pid_.set_parameters(std::stod(params["roll_kp"]),
                             std::stod(params["roll_ki"]),
                             std::stod(params["roll_kd"]));
    
    roll_pid_.set_integral_band(M_PI/40.0);
    roll_pid_.set_is_angle(true);

    //////////////////////

    pitch_pid_.set_parameters(std::stod(params["pitch_kp"]),
                             std::stod(params["pitch_ki"]),
                             std::stod(params["pitch_kd"]));
    
    pitch_pid_.set_integral_band(M_PI/40.0);
    pitch_pid_.set_is_angle(true);

    /////////////////////

    yaw_pid_.set_parameters(std::stod(params["yaw_kp"]),
                             std::stod(params["yaw_ki"]),
                             std::stod(params["yaw_kd"]));
    
    yaw_pid_.set_integral_band(M_PI/40.0);
    yaw_pid_.set_is_angle(true);
    
    //////////
    
    exec = std::make_shared<JSBSim::FGFDMExec>();

    exec->SetDebugLevel(0);
    exec->SetRootDir(info["JSBSIM_ROOT"]);
    exec->SetAircraftPath("/aircraft");
    exec->SetEnginePath("/engine");
    exec->SetSystemsPath("/systems");

    exec->LoadScript("/scripts/"+info["script_name"]);

    exec->SetRootDir(info["log_dir"]);
    exec->SetRootDir(info["JSBSIM_ROOT"]);

    JSBSim::FGInitialCondition *ic=exec->GetIC();
    if (info.count("latitude") > 0) {
        ic->SetLatitudeDegIC(std::stod(info["latitude"]));
    }
    if (info.count("longitude") > 0) {
        ic->SetLongitudeDegIC(std::stod(info["longitude"]));
    }
    if (info.count("heading") > 0) {
        angles_to_jsbsim_.set_angle(std::stod(info["heading"]));
        ic->SetPsiDegIC(angles_to_jsbsim_.angle());
    }
    if (info.count("altitude") > 0) {
        double alt_asl_meters = std::stod(info["altitude"]);
        ic->SetAltitudeASLFtIC(alt_asl_meters * meters2feet);
    }

    exec->RunIC();
    exec->Setdt(std::stod(info["dt"]));
    exec->Run();

    // Get references to each of the nodes that hold properties that we
    // care about
    JSBSim::FGPropertyManager* mgr = exec->GetPropertyManager();
    longitude_node_ = mgr->GetNode("position/long-gc-deg");
    latitude_node_ = mgr->GetNode("position/lat-gc-deg");
    altitude_node_ = mgr->GetNode("position/h-sl-ft");

    roll_node_ = mgr->GetNode("attitude/roll-rad");
    pitch_node_ = mgr->GetNode("attitude/pitch-rad");
    yaw_node_ = mgr->GetNode("attitude/heading-true-rad");

    ap_aileron_cmd_node_ = mgr->GetNode("ap/aileron_cmd");
    ap_elevator_cmd_node_ = mgr->GetNode("ap/elevator_cmd");
    ap_rudder_cmd_node_ = mgr->GetNode("ap/rudder_cmd");
    ap_throttle_cmd_node_ = mgr->GetNode("ap/throttle-cmd-norm");

    vel_north_node_ = mgr->GetNode("velocities/v-north-fps");
    vel_east_node_ = mgr->GetNode("velocities/v-east-fps");
    vel_down_node_ = mgr->GetNode("velocities/v-down-fps");

    u_vel_node_ = mgr->GetNode("velocities/u-fps");
    
    // Save state
    parent_.lock()->projection()->Forward(latitude_node_->getDoubleValue(),
                                  longitude_node_->getDoubleValue(),
                                  altitude_node_->getDoubleValue() * feet2meters,
                                  state_->pos()(0), state_->pos()(1), state_->pos()(2));

    angles_from_jsbsim_.set_angle(ang::rad2deg(yaw_node_->getDoubleValue()));

    state_->quat().set(roll_node_->getDoubleValue(),
                      -pitch_node_->getDoubleValue(),
                      ang::deg2rad(angles_from_jsbsim_.angle()));

    state_->vel() << vel_east_node_->getDoubleValue() * feet2meters,
        vel_north_node_->getDoubleValue() * feet2meters,
        -vel_down_node_->getDoubleValue() * feet2meters;

    return true;
}

bool JSBSimControl::step(double time, double dt)
{
    //ap_aileron_cmd_node_->setDoubleValue(0.9); // Right: +, Left: -
    //ap_elevator_cmd_node_->setDoubleValue(-0.5); // Down: +, Up: -
    //double u_vel = u_vel_node_->getDoubleValue();
    
    ap_throttle_cmd_node_->setDoubleValue(0.9);

    Eigen::Vector3d &u = std::static_pointer_cast<Controller>(parent_.lock()->controllers().back())->u();
    double u_roll = u(0);
    double u_pitch = u(1);
    double u_yaw = u(2);

    // Roll stabilizer
    ap_aileron_cmd_node_->setDoubleValue(u_roll);    
    
    // Pitch stabilizer
    if (time < 5) {
        ap_elevator_cmd_node_->setDoubleValue(u_pitch);    
    } else if (time < 7) {
        ap_elevator_cmd_node_->setDoubleValue(0.5);    
    } else {
        ap_elevator_cmd_node_->setDoubleValue(-0.5);    
    }

    // Yaw stabilizer
    ap_rudder_cmd_node_->setDoubleValue(u_yaw);    

    exec->Setdt(dt);
    exec->Run();

    ///////////////////////////////////////////////////////////////////////////
    // Save state
    parent_.lock()->projection()->Forward(latitude_node_->getDoubleValue(),
                                  longitude_node_->getDoubleValue(),
                                  altitude_node_->getDoubleValue() * feet2meters,
                                  state_->pos()(0), state_->pos()(1), state_->pos()(2));

    angles_from_jsbsim_.set_angle(ang::rad2deg(yaw_node_->getDoubleValue()));

    state_->quat().set(roll_node_->getDoubleValue(),
                      -pitch_node_->getDoubleValue(),
                      ang::deg2rad(angles_from_jsbsim_.angle()));

    return true;
}
