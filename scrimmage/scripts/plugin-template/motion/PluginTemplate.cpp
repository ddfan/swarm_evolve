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
#include <iostream>

#include <scrimmage/common/Utilities.h>
#include <scrimmage/common/Entity.h>
#include <scrimmage/math/State.h>

#include "(>>>PLUGIN_NAME<<<).h"

using std::cout;
using std::endl;

REGISTER_PLUGIN(MotionModel, (>>>PLUGIN_NAME<<<))

enum ModelParams
{
    X = 0,
    Y,
    THETA,
    MODEL_NUM_ITEMS
};

enum ControlParams
{
    FORWARD_VELOCITY = 0,
    TURN_RATE,
    CONTROL_NUM_ITEMS
};

(>>>PLUGIN_NAME<<<)::(>>>PLUGIN_NAME<<<)()
{
    x_.resize(MODEL_NUM_ITEMS);
    u_.resize(CONTROL_NUM_ITEMS);

    heading_pid_.set_parameters(0.1, 0.001, 0.2);
}

std::string (>>>PLUGIN_NAME<<<)::name()
{
     return std::string("(>>>PLUGIN_NAME<<<)_plugin");
}

std::tuple<int,int,int> (>>>PLUGIN_NAME<<<)::version()
{
    return std::tuple<int,int,int>(0,0,1);
}

void (>>>PLUGIN_NAME<<<)::init(std::map<std::string, std::string> &info,
                               std::map<std::string, std::string> &params)
{
    x_[X] = std::stod(info["x"]); //x
    x_[Y] = std::stod(info["y"]); //y
    x_[THETA] = Angles::deg2rad(Angles::angle_360(std::stod(info["heading"])));

    altitude_ = std::stod(info["altitude"]);

    /////////
    state_.velocity_.x_ = 0;
    state_.velocity_.y_ = 0;
    state_.velocity_.z_ = 0;

    state_.position_.x_ = x_[X];
    state_.position_.y_ = x_[Y];
    state_.position_.z_ = altitude_;

    state_.orientation_ = scrimmage::Quaternion(0, 0, x_[THETA]);
}

bool (>>>PLUGIN_NAME<<<)::step(double t, double dt)
{
    action_.clear(); // reset the action every step

    return true;
}

void (>>>PLUGIN_NAME<<<)::model(const vector_t &x , vector_t &dxdt , double t)
{
    /// 0 : x-position
    /// 1 : y-position
    /// 2 : theta
	  
    double u = u_[FORWARD_VELOCITY];
    double u_theta = u_[TURN_RATE];
    double L = 100;    

    // Saturate wheel angle:
    if (u_theta >= M_PI/4) {
        u_theta = M_PI/4 - 0.0001;
    } else if (u_theta <= -M_PI/4) {
        u_theta = -M_PI/4 + 0.0001;
    }
     
    dxdt[X] = u*cos(x[THETA]);
    dxdt[Y] = u*sin(x[THETA]);
    dxdt[THETA] = u/L*tan(u_theta);
}

scrimmage::State & (>>>PLUGIN_NAME<<<)::state()
{                
    return state_;
}

