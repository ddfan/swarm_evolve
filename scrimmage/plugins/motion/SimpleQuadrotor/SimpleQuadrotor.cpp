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
#include "SimpleQuadrotor.h"
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/math/Angles.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/entity/Entity.h>

REGISTER_PLUGIN(scrimmage::MotionModel, SimpleQuadrotor, 
                SimpleQuadrotor_plugin)

namespace pl = std::placeholders;
namespace sc = scrimmage;
using ang = sc::Angles;

enum ModelParams
{
    X = 0,
    Y,
    Z,
    XDOT,
    YDOT,
    ZDOT,
    YAW,
    YAWDOT,
    MODEL_NUM_ITEMS
};

enum ControlParams
{
    X_THRUST = 0,
    Y_THRUST,
    Z_THRUST,
    TURN_RATE,
    CONTROL_NUM_ITEMS
};

SimpleQuadrotor::SimpleQuadrotor()
{ 
    x_.resize(MODEL_NUM_ITEMS);
}

bool SimpleQuadrotor::init(std::map<std::string, std::string> &info,
                           std::map<std::string, std::string> &params)
{     
    x_[X] = std::stod(info["x"]); //x
    x_[Y] = std::stod(info["y"]); //y
    x_[Z] = std::stod(info["z"]); //z
    x_[XDOT] = 0; // x-velocity
    x_[YDOT] = 0; // y-velocity
    x_[ZDOT] = 0; // z-velocity
    x_[YAW] = ang::deg2rad(std::stod(info["heading"])); //yaw     
    x_[YAWDOT] = 0; // yaw-velocity
    
    state_->pos() << x_[X], x_[Y], x_[Z];     
    state_->quat().set(0, x_[XDOT] / 100.0, x_[YAW]);
    state_->vel() << x_[XDOT], x_[YDOT], x_[ZDOT];

    max_vel_ = std::stod(params["max_vel"]);
    max_pitch_ = ang::deg2rad(std::stod(params["max_pitch"]));

    return true;
}

bool SimpleQuadrotor::step(double time, double dt)
{    
    ode_step(dt);
    
    state_->pos() << x_[X], x_[Y], x_[Z];     

    double pct_of_max_vel = std::min(1.0, x_[XDOT] / max_vel_);
    double pitch = max_pitch_ * pct_of_max_vel;
    state_->quat().set(0, pitch, x_[YAW]);
    state_->vel() << x_[XDOT] * cos(x_[YAW]), x_[XDOT] * sin(x_[YAW]), x_[ZDOT];

    return true;
}

void SimpleQuadrotor::model(const vector_t &x , vector_t &dxdt , double t)
{
    /// 0 : x-position
    /// 1 : y-position
    /// 2 : z-position
    /// 3 : x-velocity
    /// 4 : y-velocity
    /// 5 : z-velocity
    /// 6 : yaw
    /// 7 : yaw-velocity
     
    Eigen::Vector4d &u = std::static_pointer_cast<Controller>(parent_.lock()->controllers().back())->u();
    double max_x_velocity = 15;              
    double max_z_velocity = 10;
     
    double x_thrust = u[X_THRUST];
    double y_thrust = u[Y_THRUST];
    double z_thrust = u[Z_THRUST];
    double turn_force = u[TURN_RATE];

    double x_velocity = x[XDOT];
    if (x_velocity > max_x_velocity) {
        x_velocity = max_x_velocity;
    }

    double z_velocity = x[ZDOT];
    if (z_velocity > max_z_velocity) {
        z_velocity = max_z_velocity;
    }
     
    dxdt[X]      = x_velocity * cos(x[YAW]);
    dxdt[Y]      = x_velocity * sin(x[YAW]);
    dxdt[Z]      = z_velocity;
    dxdt[XDOT]   = x_thrust;
    dxdt[YDOT]   = y_thrust;
    dxdt[ZDOT]   = z_thrust;
    dxdt[YAW]    = x[YAWDOT];
    dxdt[YAWDOT] = turn_force;
}
