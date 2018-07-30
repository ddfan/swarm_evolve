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
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/math/State.h>
#include <scrimmage/parse/ParseUtils.h>

#include "Straight.h"

#include <iostream>
using std::cout;
using std::endl;

REGISTER_PLUGIN(scrimmage::Autonomy, Straight, Straight_plugin)

Straight::Straight()
{
}

void Straight::init(std::map<std::string,std::string> &params)
{
    speed_ = scrimmage::get("speed", params, 0.0);

    desired_state_->vel() = speed_*Eigen::Vector3d::UnitX();
    desired_state_->quat().set(0,0,state_->quat().yaw());
    desired_state_->pos() = (state_->pos()(2))*Eigen::Vector3d::UnitZ();

    // Project goal in front...
    Eigen::Vector3d rel_pos = Eigen::Vector3d::UnitX()*2000;
    Eigen::Vector3d unit_vector = rel_pos.normalized();
    unit_vector = state_->quat().rotate(unit_vector);
    goal_ = state_->pos() + unit_vector * rel_pos.norm();
}

bool Straight::step_autonomy(double t, double dt)
{
    Eigen::Vector3d diff = goal_ - state_->pos();
    Eigen::Vector3d v = speed_ * diff.normalized();

    ///////////////////////////////////////////////////////////////////////////
    // Convert desired velocity to desired speed, heading, and pitch controls
    ///////////////////////////////////////////////////////////////////////////
    desired_state_->vel()(0) = v.norm();

    // Desired heading
    double heading = scrimmage::Angles::angle_2pi(atan2(v(1), v(0)));

    // Desired pitch
    Eigen::Vector2d xy(v(0), v(1));
    double pitch = scrimmage::Angles::angle_2pi(atan2(v(2), xy.norm()));

    // Set Desired Altitude to goal's z-position
    desired_state_->pos()(2) = goal_(2);

    // Set the desired pitch and heading
    desired_state_->quat().set(0, pitch, heading);

    return true;
}
