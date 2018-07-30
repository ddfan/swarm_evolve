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
#include <memory>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>
#include <memory>

#include <scrimmage/entity/Entity.h>
#include <scrimmage/math/State.h>

#include <scrimmage/pubsub/Message.h>
#include <scrimmage/msgs/Collision.pb.h>
#include <scrimmage/autonomy/Autonomy.h>
#include <scrimmage/common/RTree.h>

#include "SimpleCollision.h"

namespace sc = scrimmage;
namespace sm = scrimmage_msgs;

REGISTER_PLUGIN(scrimmage::EntityInteraction, SimpleCollision,
                SimpleCollision_plugin)

SimpleCollision::SimpleCollision()
{
}

bool SimpleCollision::init(std::map<std::string,std::string> &mission_params,
                           std::map<std::string,std::string> &plugin_params)
{
    collision_range_ = sc::get("collision_range", plugin_params, 0.0);
    startup_collisions_only_ = sc::get("startup_collisions_only", plugin_params, false);

    enable_team_collisions_ = sc::get<bool>("enable_friendly_collisions", plugin_params, true);
    enable_non_team_collisions_ = sc::get<bool>("enable_enemy_collisions", plugin_params, true);

    init_alt_deconflict_ = sc::get<bool>("init_alt_deconflict", plugin_params, false);
    
    // Setup publishers
    team_collision_pub_ = create_publisher("TeamCollision");
    non_team_collision_pub_ = create_publisher("NonTeamCollision");

    return true;
}


bool SimpleCollision::step_entity_interaction(std::list<sc::EntityPtr> &ents,
                                              double t, double dt)
{
    if (startup_collisions_only_) {
        return true;
    }

    // Account for entities "colliding"
    for (sc::EntityPtr ent1 : ents) {
        Eigen::Vector3d p1 = ent1->state()->pos();
        for (sc::EntityPtr ent2 : ents) {
            // ignore distance between itself
            if (ent1->id().id() == ent2->id().id()) continue;

            // ignore collisions that have already occurred this time-step
            if (!ent1->is_alive() || !ent2->is_alive()) continue;

            Eigen::Vector3d p2 = ent2->state()->pos();

            double dist = (p1 - p2).norm();
            if (dist < collision_range_) {
                if (enable_team_collisions_ &&
                    ent1->id().team_id() == ent2->id().team_id()) {

                    ent1->collision();
                    ent2->collision();

                    auto msg = std::make_shared<sc::Message<sm::TeamCollision>>();
                    msg->data.set_entity_id_1(ent1->id().id());
                    msg->data.set_entity_id_2(ent2->id().id());
                    publish_immediate(t, team_collision_pub_, msg);

                } else if (enable_non_team_collisions_) {
                    ent1->collision();
                    ent2->collision();

                    auto msg = std::make_shared<sc::Message<sm::NonTeamCollision>>();
                    msg->data.set_entity_id_1(ent1->id().id());
                    msg->data.set_entity_id_2(ent2->id().id());
                    publish_immediate(t, non_team_collision_pub_, msg);
                }
            }
        }
    }
    return true;
}

bool SimpleCollision::collision_exists(std::list<sc::EntityPtr> &ents,
                                       Eigen::Vector3d &p)
{
    if (ents.empty()) {
        return false;
    } else {
        if (init_alt_deconflict_) {
            for (sc::EntityPtr ent : ents) {                
                if (std::abs(p(2) - ent->state()->pos()(2)) <= collision_range_) {
                    return true;
                }
            }
        } else {
            std::vector<sc::ID> neighbors;
            if (ents.front()->autonomies().empty()) {
                return false;
            } else {
                sc::RTreePtr rtree = ents.front()->autonomies().front()->rtree();
                rtree->neighbors_in_range(p, neighbors, collision_range_); 
                return !neighbors.empty();
            }
        }
    }    
    return false;
}
