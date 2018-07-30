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
#include "SimpleCar.h"
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/math/Angles.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/entity/Entity.h>

namespace sc = scrimmage;
namespace pl = std::placeholders;

REGISTER_PLUGIN(scrimmage::MotionModel, SimpleCar, SimpleCar_plugin)

enum ModelParams
{
    X = 0,
    Y,
    Z,
    Z_dot,
    THETA,    
    MODEL_NUM_ITEMS
};

enum ControlParams
{
    FORWARD_VELOCITY = 0,
    TURN_RATE,
    CONTROL_NUM_ITEMS
};

SimpleCar::SimpleCar()
{   
    x_.resize(MODEL_NUM_ITEMS);
}

bool SimpleCar::init(std::map<std::string, std::string> &info,
                     std::map<std::string, std::string> &params)
{       
    x_[X] = std::stod(info["x"]); //x
    x_[Y] = std::stod(info["y"]); //y
    x_[Z] = std::stod(info["z"]); //y
    x_[Z_dot] = 0;
    x_[THETA] = sc::Angles::deg2rad(std::stod(info["heading"]));    
        
    length_ = sc::get<double>("length", params, 100.0);
    mass_ = sc::get<double>("mass", params, 1.0);
    enable_gravity_ = sc::get<bool>("enable_gravity", params, false);

    /////////
    state_->vel() << 0, 0, 0;
    state_->pos() << x_[X], x_[Y], x_[Z];
    state_->quat().set(0, 0, x_[THETA]);
    
    return true;
}

bool SimpleCar::step(double time, double dt)
{       
    double prev_x = x_[X];
    double prev_y = x_[Y];
    double prev_z = x_[Z];
    
    ode_step(dt);

    ext_force_ = Eigen::Vector3d::Zero();
    
    /////////////////////
    // Save state
    // Simple velocity
    state_->vel() << (x_[X] - prev_x) / dt, (x_[Y] - prev_y) / dt,
        (x_[Z] - prev_z) / dt;
    
    state_->pos() << x_[X], x_[Y], x_[Z];
    state_->quat().set(0, 0, x_[THETA]);
    return true;
}

#include <iostream>

void SimpleCar::model(const vector_t &x , vector_t &dxdt , double t)
{
    /// 0 : x-position
    /// 1 : y-position
    /// 2 : theta
	  
    Eigen::Vector2d &u = std::static_pointer_cast<Controller>(parent_.lock()->controllers().back())->u();
    double u_vel = u(FORWARD_VELOCITY);
    double u_theta = u(TURN_RATE);
    
    // Saturate wheel angle:
    if (u_theta >= M_PI/4) {
        u_theta = M_PI/4 - 0.0001;
    } else if (u_theta <= -M_PI/4) {
        u_theta = -M_PI/4 + 0.0001;
    }
     
    dxdt[X] = u_vel*cos(x[THETA]);
    dxdt[Y] = u_vel*sin(x[THETA]);
    dxdt[THETA] = u_vel/length_*tan(u_theta);

    if (enable_gravity_) {
        dxdt[Z] = x[Z_dot];
        dxdt[Z_dot] = mass_ * -9.8;
    } else {
        dxdt[Z] = 0;
        dxdt[Z_dot] = 0;
    }
    
    // Saturate based on external force:
    if (std::abs(ext_force_(0)) > 0.1) {        
        dxdt[X] = 0;
    }

    if (std::abs(ext_force_(1)) > 0.1) {
        dxdt[Y] = 0;
    }

    if (std::abs(ext_force_(2)) > 0.1) {
        dxdt[Z] = 0;
    }
}
