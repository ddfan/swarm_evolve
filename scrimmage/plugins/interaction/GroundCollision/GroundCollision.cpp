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
#include <limits>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/parse/MissionParse.h>
#include <memory>

#include <scrimmage/entity/Entity.h>
#include <scrimmage/math/State.h>

#include <scrimmage/pubsub/Message.h>
#include <scrimmage/msgs/Collision.pb.h>

#include <GeographicLib/LocalCartesian.hpp>

#include "GroundCollision.h"

namespace sc = scrimmage;
namespace sm = scrimmage_msgs;

REGISTER_PLUGIN(scrimmage::EntityInteraction, GroundCollision,
                GroundCollision_plugin)

GroundCollision::GroundCollision()
{
}

bool GroundCollision::init(std::map<std::string,std::string> &mission_params,
                           std::map<std::string,std::string> &plugin_params)
{
    // Determine ground collision z-value. If ground_collision_altitude is
    // defined, it has priority.
    ground_collision_z_ = sc::get("ground_collision_z", plugin_params, 0.0);
    if (plugin_params.count("ground_collision_altitude") > 0 && mp_) {
        double x,y,z,alt;
        alt = sc::get("ground_collision_altitude", plugin_params, 0.0);
        proj_->Forward(mp_->latitude_origin(), mp_->longitude_origin(), alt, x, y, z);
        ground_collision_z_ = z;
    }

    collision_pub_ = create_publisher("GroundCollision");

    return true;
}


bool GroundCollision::step_entity_interaction(std::list<sc::EntityPtr> &ents,
                                              double t, double dt)
{
    // Account for entities colliding with
    for (sc::EntityPtr ent : ents) {
        if (ent->state()->pos()(2) < ground_collision_z_) {
            ent->collision();

            auto msg = std::make_shared<sc::Message<sm::GroundCollision>>();
            msg->data.set_entity_id(ent->id().id());
            publish_immediate(t, collision_pub_, msg);
        }
    }
    return true;
}

bool GroundCollision::collision_exists(std::list<sc::EntityPtr> &ents,
                                       Eigen::Vector3d &p)
{
    if (p(2) < ground_collision_z_) {
        return true;
    }
    return false;
}
