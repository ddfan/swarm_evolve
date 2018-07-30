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
#include <fstream>

#include <scrimmage/metrics/Metrics.h>

#include <scrimmage/entity/Entity.h>
#include <scrimmage/math/State.h>
#include <scrimmage/common/RTree.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>

#include <scrimmage/pubsub/Message.h>
#include <scrimmage/pubsub/Subscriber.h>
#include <scrimmage/msgs/Collision.pb.h>
#include <scrimmage/msgs/Event.pb.h>

#include "SimpleCollisionMetrics.h"

REGISTER_PLUGIN(scrimmage::Metrics, SimpleCollisionMetrics, SimpleCollisionMetrics_plugin)

namespace sc = scrimmage;
namespace sm = scrimmage_msgs;
using std::cout;
using std::endl;

SimpleCollisionMetrics::SimpleCollisionMetrics()
{
}

void SimpleCollisionMetrics::init(std::map<std::string,std::string> &params)
{
    params_ = params;

    sub_team_collision_ = create_subscriber("TeamCollision");
    sub_non_team_collision_ = create_subscriber("NonTeamCollision");
    sub_ground_collision_ = create_subscriber("GroundCollision");
    sub_ent_gen_ = create_subscriber("EntityGenerated");
    sub_ent_rm_ = create_subscriber("EntityRemoved");
    sub_ent_pres_end_ = create_subscriber("EntityPresentAtEnd");        
}

bool SimpleCollisionMetrics::step_metrics(double t, double dt)
{
    for (auto msg : sub_team_collision_->pop_msgs<sc::Message<sm::TeamCollision>>()) {
        scores_[msg->data.entity_id_1()].increment_team_collisions();
        scores_[msg->data.entity_id_2()].increment_team_collisions();
    }

    for (auto msg : sub_non_team_collision_->pop_msgs<sc::Message<sm::NonTeamCollision>>()) {
        scores_[msg->data.entity_id_1()].increment_non_team_collisions();
        scores_[msg->data.entity_id_2()].increment_non_team_collisions();
    }

    for (auto msg : sub_ground_collision_->pop_msgs<sc::Message<sm::GroundCollision>>()) {
        scores_[msg->data.entity_id()].increment_ground_collisions();
    }

    for (auto msg : sub_ent_gen_->pop_msgs<sc::Message<sm::EntityGenerated>>()) {
        scores_[msg->data.entity_id()].set_flight_time_start(t);
    }

    for (auto msg : sub_ent_rm_->pop_msgs<sc::Message<sm::EntityRemoved>>()) {
        scores_[msg->data.entity_id()].set_flight_time_end(t);
    }

    for (auto msg : sub_ent_pres_end_->pop_msgs<sc::Message<sm::EntityPresentAtEnd>>()) {
        scores_[msg->data.entity_id()].set_flight_time_end(t);
        surviving_teams_[(*team_lookup_)[msg->data.entity_id()]] = true;
    }

    return true;
}

void SimpleCollisionMetrics::calc_team_scores() {
    double end_time = -std::numeric_limits<double>::infinity();
    double beg_time = std::numeric_limits<double>::infinity();
    for (auto &kv : scores_) {
        double temp_beg_time = kv.second.flight_time_start();
        double temp_end_time = kv.second.flight_time_end();
        if (temp_beg_time < beg_time) {
            beg_time = temp_beg_time;
        }
        if (temp_end_time > end_time) {
            end_time = temp_end_time;
        }
    }

    double max_flight_time = end_time - beg_time;

    for (auto &kv : scores_) {

        SimpleCollisionScore &score = kv.second;

        // Set the max flight time for each score:
        score.set_max_flight_time(max_flight_time);

        int team_id = (*team_lookup_)[kv.first];

        // Create the score, if necessary
        if (team_coll_scores_.count(team_id) == 0) {
            SimpleCollisionScore score;
            score.set_weights(params_);
            score.set_max_flight_time(max_flight_time);
            team_coll_scores_[team_id] = score;
        }
        team_coll_scores_[team_id].add_non_team_collisions(score.non_team_collisions());
        team_coll_scores_[team_id].add_team_collisions(score.team_collisions());
        team_coll_scores_[team_id].add_ground_collisions(score.ground_collisions());
        team_coll_scores_[team_id].add_flight_time(score.flight_time());
        team_coll_scores_[team_id].increment_entity_count();
    }
    
    for (auto &kv : team_coll_scores_) {
        int team_id = kv.first;
        SimpleCollisionScore &score = kv.second;
        team_metrics_[team_id]["entity_count"] = score.entity_count();
        team_metrics_[team_id]["flight_time"] = score.flight_time();
        team_metrics_[team_id]["flight_time_norm"] = score.flight_time_norm();
        team_metrics_[team_id]["non_team_coll"] = score.non_team_collisions();
        team_metrics_[team_id]["team_coll"] = score.team_collisions();
        team_metrics_[team_id]["ground_coll"] = score.ground_collisions();

        team_scores_[team_id] = score.score();
    }

    // list the headers we want put in the csv file
    headers_.push_back("entity_count");
    headers_.push_back("flight_time");
    headers_.push_back("flight_time_norm");
    headers_.push_back("non_team_coll");
    headers_.push_back("team_coll");
    headers_.push_back("ground_coll");
}

void SimpleCollisionMetrics::print_team_summaries()
{
    for (std::map<int, SimpleCollisionScore>::iterator it = team_coll_scores_.begin();
         it != team_coll_scores_.end(); it++) {

        bool survived = false;
        auto it_survive = surviving_teams_.find(it->first);
        if (it_survive != surviving_teams_.end()) survived = true;
        
        cout << "Team ID: " << it->first;
        if (survived) {
            cout << "\t(Survived round)" << endl;
        } else {
            cout << "\t(Didn't survive round)" << endl;            
        }
        cout << "Score: " << it->second.score() << endl;        
        cout << "Entity Count: " << it->second.entity_count() << endl;
        cout << "Total Flight Time: " << it->second.flight_time() << endl;
        cout << "Total Normalized Flight Time: " << it->second.flight_time_norm() << endl;
        cout << "Non-Team Collisions: " << it->second.non_team_collisions() << endl;
        cout << "Team Collisions: " << it->second.team_collisions() << endl;
        cout << "Ground Collisions: " << it->second.ground_collisions() << endl;
        cout << sc::generate_chars("-",70) << endl;
    }
}
