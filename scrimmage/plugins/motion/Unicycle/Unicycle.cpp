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
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/math/State.h>
#include <scrimmage/entity/Entity.h>
#include <boost/algorithm/clamp.hpp>

#include "Unicycle.h"

REGISTER_PLUGIN(scrimmage::MotionModel, Unicycle, 
                Unicycle_plugin)

namespace sc = scrimmage;

namespace pl = std::placeholders;

enum ModelParams
{
    X = 0,
    Y,
    THETA,
    MODEL_NUM_ITEMS
};

Unicycle::Unicycle()
{
    x_.resize(MODEL_NUM_ITEMS);
}

bool Unicycle::init(std::map<std::string, std::string> &info,
                            std::map<std::string, std::string> &params)
{
    x_[X] = state_->pos()(0);
    x_[Y] = state_->pos()(1);
    x_[THETA] = state_->quat().yaw();

    turn_rate_max_ = std::stod(params.at("turn_rate_max"));
    vel_max_ = std::stod(params.at("vel_max"));
    return true;
}

bool Unicycle::step(double t, double dt)
{   
    u_ = std::static_pointer_cast<Controller>(parent_.lock()->controllers().back())->u();    
    
    double prev_x = x_[X];
    double prev_y = x_[Y];
    
    ode_step(dt);
    
    double dx = (x_[X] - prev_x) / dt;
    double dy = (x_[Y] - prev_y) / dt;

    state_->vel()(0) = dx;
    state_->vel()(1) = dy;

    state_->pos()(0) = x_[X];
    state_->pos()(1) = x_[Y];

    state_->quat().set(0, 0, x_[THETA]);

    return true;
}

void Unicycle::model(const vector_t &x , vector_t &dxdt , double t)
{    
    double vel = boost::algorithm::clamp(u_(0), -vel_max_, vel_max_);
    double yaw_rate = boost::algorithm::clamp(u_(1), -turn_rate_max_, turn_rate_max_);
    dxdt[X] = vel * cos(x[THETA]);
    dxdt[Y] = vel * sin(x[THETA]);
    dxdt[THETA] = yaw_rate;
}
