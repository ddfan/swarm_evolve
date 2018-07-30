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
#include <memory>

#include <scrimmage/metrics/Metrics.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/pubsub/Subscriber.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>

#include "CaptureTheFlagMetrics.h"

#include <scrimmage/pubsub/Message.h>
#include <scrimmage-gtri-share/msgs/Fire.pb.h>
#include <scrimmage-gtri-share/msgs/BaseCollision.pb.h>

using std::cout;
using std::endl;

namespace sc = scrimmage;
namespace sgs = scrimmage_gtri_share;

REGISTER_PLUGIN(scrimmage::Metrics, CaptureTheFlagMetrics, CaptureTheFlagMetrics_plugin)

CaptureTheFlagMetrics::CaptureTheFlagMetrics() {}

void CaptureTheFlagMetrics::init(std::map<std::string,std::string> &params)
{
    params_ = params;

    sub_base_coll_ = create_subscriber("BaseCollision");
    sub_fire_result_ = create_subscriber("FireResult");
    sub_distfrombase_ = create_subscriber("DistanceFromBase");

}

bool CaptureTheFlagMetrics::step_metrics(double t, double dt)
{    
    for (auto msg : sub_base_coll_->pop_msgs<sc::Message<sgs::BaseCollision>>()) {
        if (msg->data.type() == sgs::BaseCollision::Enemy) {            
            scores_[msg->data.entity_id()].increment_enemy_base_collisions();
        }
    }

    for (auto msg : sub_fire_result_->pop_msgs<sc::Message<sgs::FireResult>>()) {
        if (msg->data.type() == sgs::FireResult::EnemyKill) {            
            scores_[msg->data.source_id()].increment_air_to_air_kills();
        }
    }

    for (auto msg : sub_distfrombase_->pop_msgs<sc::Message<sgs::DistanceFromBase>>()) {
        scores_[msg->data.entity_id()].add_distance_from_base(msg->data.distance());
    }

    return true;
}

void CaptureTheFlagMetrics::print_team_summaries()
{
    for (std::map<int, CaptureTheFlagScore>::iterator it = ctf_scores_.begin();
         it != ctf_scores_.end(); it++) {        
        cout << "Team ID: " << it->first << endl;
        cout << "Score: " << it->second.score() << endl;
        cout << "Air-to-Air Kills: " << it->second.air_to_air_kills() << endl;
        cout << "Enemy Base Collisions: " << it->second.enemy_base_collisions() << endl;
        cout << "Total Distance From Enemy Bases: " << it->second.distance_from_base() << endl;
        cout << sc::generate_chars("-",70) << endl;
    }
}

void CaptureTheFlagMetrics::calc_team_scores() {
    for (auto &kv : scores_) {
        CaptureTheFlagScore &score = kv.second;
        
        int team_id = (*team_lookup_)[kv.first];

        // Create the score, if necessary
        if (ctf_scores_.count(team_id) == 0) {
            CaptureTheFlagScore score;
            score.set_weights(params_);
            ctf_scores_[team_id] = score;
        }
        ctf_scores_[team_id].add_enemy_kills(score.air_to_air_kills());
        ctf_scores_[team_id].add_enemy_base_collisions(score.enemy_base_collisions());
        ctf_scores_[team_id].add_distance_from_base(score.distance_from_base());
    }

    for (auto &kv : ctf_scores_) {
        int team_id = kv.first;
        CaptureTheFlagScore &score = kv.second;

        team_metrics_[team_id]["air_to_air_kills"] = score.air_to_air_kills();
        team_metrics_[team_id]["enemy_base_coll"] = score.enemy_base_collisions();
        team_metrics_[team_id]["distance_from_base"] = score.distance_from_base();
        
        team_scores_[team_id] = score.score();
    }

    // list the headers we want put in the csv file
    headers_.push_back("air_to_air_kills");
    headers_.push_back("enemy_base_coll");
    headers_.push_back("distance_from_base");
}
