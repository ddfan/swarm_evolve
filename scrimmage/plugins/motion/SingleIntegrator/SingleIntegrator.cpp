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
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/math/State.h>
#include <scrimmage/entity/Entity.h>

#include "SingleIntegrator.h"

REGISTER_PLUGIN(scrimmage::MotionModel, SingleIntegrator,
                SingleIntegrator_plugin)

namespace sc = scrimmage;

namespace pl = std::placeholders;

enum ModelParams
{
    X = 0,
    Y,
    Z,
    HEADING,
    PITCH,
    MODEL_NUM_ITEMS
};

SingleIntegrator::SingleIntegrator()
{
    x_.resize(MODEL_NUM_ITEMS);
}

bool SingleIntegrator::init(std::map<std::string, std::string> &info,
                            std::map<std::string, std::string> &params)
{
    x_[X] = std::stod(info["x"]); //x
    x_[Y] = std::stod(info["y"]); //y
    x_[Z] = std::stod(info["z"]); //z

    x_[HEADING] = sc::Angles::deg2rad(std::stod(info["heading"]));
    x_[PITCH] = 0;

    state_->vel() << 0, 0, 0;
    state_->pos() << x_[X], x_[Y], x_[Z];
    state_->quat().set(0, x_[PITCH], x_[HEADING]);

    return true;
}

#include <iostream>
using std::cout;
using std::endl;

bool SingleIntegrator::step(double t, double dt)
{
    u_ = std::static_pointer_cast<Controller>(parent_.lock()->controllers().back())->u();

    double prev_x = x_[X];
    double prev_y = x_[Y];
    double prev_z = x_[Z];

    ode_step(dt);

    Eigen::Vector3d &vel = state_->vel();
    double dx = (x_[X] - prev_x) / dt;
    double dy = (x_[Y] - prev_y) / dt;
    double dz = (x_[Z] - prev_z) / dt;

    vel << dx, dy, dz;
    state_->pos() << x_[X], x_[Y], x_[Z];

    Eigen::Vector2d xy(dx, dy);

    double yaw = atan2(dy, dx);
    double pitch = atan2(dz, xy.norm());
    state_->quat().set(0, pitch, yaw);

    return true;
}

void SingleIntegrator::model(const vector_t &x , vector_t &dxdt , double t)
{
    dxdt[X] = u_(X);
    dxdt[Y] = u_(Y);
    dxdt[Z] = u_(Z);
}
