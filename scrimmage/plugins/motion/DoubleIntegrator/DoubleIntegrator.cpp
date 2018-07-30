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
#include "DoubleIntegrator.h"
#include <scrimmage/entity/Entity.h>
#include <boost/algorithm/clamp.hpp>

REGISTER_PLUGIN(scrimmage::MotionModel, DoubleIntegrator, DoubleIntegrator_plugin)

namespace pl = std::placeholders;
using boost::algorithm::clamp;

enum ModelParams {X, Y, Z, VX, VY, VZ, STATE_SIZE};

DoubleIntegrator::DoubleIntegrator() {
    x_.resize(STATE_SIZE);
}

bool DoubleIntegrator::init(std::map<std::string, std::string> &info,
                            std::map<std::string, std::string> &params) {
    max_vel_ = std::stod(params.at("max_vel"));
    max_acc_ = std::stod(params.at("max_acc"));

    x_[X] = state_->pos()(0);
    x_[Y] = state_->pos()(1);
    x_[Z] = state_->pos()(2);

    x_[VX] = clamp(state_->vel()(0), -max_vel_, max_vel_);
    x_[VY] = clamp(state_->vel()(1), -max_vel_, max_vel_);
    x_[VZ] = clamp(state_->vel()(2), -max_vel_, max_vel_);
    
    state_->pos() << x_[X], x_[Y], x_[Z];
    
    return true;
}

bool DoubleIntegrator::step(double t, double dt) {   
    u_ = std::static_pointer_cast<Controller>(parent_.lock()->controllers().back())->u();    
    u_(0) = clamp(u_(0), -max_acc_, max_acc_);
    u_(1) = clamp(u_(1), -max_acc_, max_acc_);
    u_(2) = clamp(u_(2), -max_acc_, max_acc_);

    ode_step(dt);
    
    x_[VX] = clamp(x_[VX], -max_vel_, max_vel_);
    x_[VY] = clamp(x_[VY], -max_vel_, max_vel_);
    x_[VZ] = clamp(x_[VZ], -max_vel_, max_vel_);

    state_->pos() << x_[X], x_[Y], x_[Z];    
    state_->vel() << x_[VX], x_[VY], x_[VZ];
    state_->quat().set(0, 0, atan2(x_[VY], x_[VX]));
    return true;
}

double DoubleIntegrator::update_dvdt(double vel, double acc) {
    if (vel >= max_vel_ && acc > 0) {
        return 0;
    } else if (vel <= -max_vel_ && acc < 0) {
        return 0;
    } else {
        return acc;
    }
}

void DoubleIntegrator::model(const vector_t &x , vector_t &dxdt , double t) {    
    dxdt[X] = clamp(x_[VX], -max_vel_, max_vel_);
    dxdt[Y] = clamp(x_[VY], -max_vel_, max_vel_);
    dxdt[Z] = clamp(x_[VZ], -max_vel_, max_vel_);

    dxdt[VX] = update_dvdt(x[VX], u_(0));
    dxdt[VY] = update_dvdt(x[VY], u_(1));
    dxdt[VZ] = update_dvdt(x[VZ], u_(2));
}
