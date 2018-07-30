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
#include "SimpleAircraft.h"
#include <scrimmage/common/Utilities.h>
#include <scrimmage/common/Random.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/math/Angles.h>
#include <boost/algorithm/clamp.hpp>
#include <scrimmage/entity/Entity.h>

using boost::algorithm::clamp;

REGISTER_PLUGIN(scrimmage::MotionModel, SimpleAircraft, SimpleAircraft_plugin)

namespace sc = scrimmage;
namespace pl = std::placeholders;

enum ModelParams
{
    X = 0,
    Y,
    Z,
    ROLL,
    PITCH,
    YAW,
    SPEED,
    MODEL_NUM_ITEMS
};

enum ControlParams
{
    THRUST = 0,
    TURN_RATE,
    PITCH_RATE,
    CONTROL_NUM_ITEMS
};

SimpleAircraft::SimpleAircraft()
{
    x_.resize(MODEL_NUM_ITEMS);
}

std::tuple<int,int,int> SimpleAircraft::version()
{
    return std::tuple<int,int,int>(0,0,1);
}

bool SimpleAircraft::init(std::map<std::string, std::string> &info,
                          std::map<std::string, std::string> &params)
{
    Eigen::Vector3d &pos = state_->pos();
    sc::Quaternion &quat = state_->quat();

    x_[X] = pos(0);
    x_[Y] = pos(1);
    x_[Z] = pos(2);
    x_[ROLL] = quat.roll();
    x_[PITCH] = quat.pitch();
    x_[YAW] = quat.yaw();
    x_[SPEED] = state_->vel().norm();

    length_ = sc::get("turning_radius", params, 50.0);

    min_velocity_ = sc::get("min_velocity", params, 15.0);
    max_velocity_ = sc::get("max_velocity", params, 40.0);
    max_roll_ = sc::Angles::deg2rad(sc::get("max_roll", params, 30.0));
    max_pitch_ = sc::Angles::deg2rad(sc::get("max_pitch", params, 30.0));
    noise_stdev_ = sc::get("noise_stdev", params, 0.01);


    state_->pos() << x_[X], x_[Y], x_[Z];
    state_->quat().set(-x_[ROLL], x_[PITCH], x_[YAW]);
    state_->vel() << x_[SPEED] * cos(x_[5]), x_[SPEED] * sin(x_[5]), 0;

    return true;
}

bool SimpleAircraft::step(double time, double dt)
{
    // Need to saturate state variables before model runs    
    x_[ROLL] = clamp(x_[ROLL], -max_roll_, max_roll_);
    x_[PITCH] = clamp(x_[PITCH], -max_pitch_, max_pitch_);
    x_[SPEED] = clamp(x_[SPEED], min_velocity_, max_velocity_);

    if (u_ == nullptr) {
        std::shared_ptr<Controller> ctrl =
            std::dynamic_pointer_cast<Controller>(parent_.lock()->controllers().back());
        if (ctrl) {
            u_ = ctrl->u();
        }
    }

    if (u_ == nullptr) {
        return false;
    }

    ode_step(dt);

    state_->pos() << x_[X], x_[Y], x_[Z];
    state_->quat().set(-x_[ROLL], x_[PITCH], x_[YAW]);
    state_->vel() << x_[SPEED] * cos(x_[YAW]), x_[SPEED] * sin(x_[YAW]), 0;

    return true;
}

void SimpleAircraft::model(const vector_t &x , vector_t &dxdt , double t)
{
    /// 0 : x-position
    /// 1 : y-position
    /// 2 : z-position
    /// 3 : roll
    /// 4 : pitch
    /// 5 : yaw
    /// 6 : speed
    double thrust = (*u_)(THRUST);
    double roll_rate = (*u_)(TURN_RATE);
    double pitch_rate = (*u_)(PITCH_RATE);

    // Saturate control inputs
    thrust = clamp(thrust, -100.0, 100.0);
    roll_rate = clamp(roll_rate, -1, 1.0);
    pitch_rate = clamp(pitch_rate, -1.0, 1.0);

    double xy_speed = x[SPEED] * cos(x[PITCH]);
    sc::RandomPtr randomptr = parent_.lock()->random();
    dxdt[X] = xy_speed*cos(x[YAW])+randomptr->rng_normal()*noise_stdev_;
    dxdt[Y] = xy_speed*sin(x[YAW])+randomptr->rng_normal()*noise_stdev_;
    dxdt[Z] = -sin(x[PITCH])*x[SPEED]+randomptr->rng_normal()*noise_stdev_;
    dxdt[ROLL] = roll_rate/2+randomptr->rng_normal()*noise_stdev_;
    dxdt[PITCH] = pitch_rate/2+randomptr->rng_normal()*noise_stdev_;
    dxdt[YAW] = x[SPEED]/length_*tan(x[ROLL])+randomptr->rng_normal()*noise_stdev_;

    dxdt[SPEED] = thrust/5+randomptr->rng_normal()*noise_stdev_;
}

void SimpleAircraft::teleport(sc::StatePtr &state)
{
    x_[X] = state->pos()[0];
    x_[Y] = state->pos()[1];
    x_[Z] = state->pos()[2];
    x_[ROLL] = state->quat().roll();
    x_[PITCH] = state->quat().pitch();
    x_[YAW] = state->quat().yaw();
    x_[SPEED] = state->vel()[0];
}
