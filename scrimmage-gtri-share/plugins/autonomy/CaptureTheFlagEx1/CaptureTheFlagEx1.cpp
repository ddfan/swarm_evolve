/******************************************************************************
* Copyright 2016, Georgia Tech Research Corporation, Atlanta, Georgia 30332
* All Rights Reserved
******************************************************************************/
#include <iostream>
#include <memory>

#include <scrimmage/entity/Entity.h>
#include <scrimmage/math/Angles.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/common/Random.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/common/RTree.h>
#include <scrimmage/math/Angles.h>
#include <scrimmage/parse/MissionParse.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/math/State.h>

#include <scrimmage-gtri-share/utilities/Utilities.h>

#include "CaptureTheFlagEx1.h"

using std::cout;
using std::endl;
using Eigen::Vector3d;

REGISTER_PLUGIN(scrimmage::Autonomy, CaptureTheFlagEx1, CaptureTheFlagEx1_plugin)

namespace sc = scrimmage;
namespace sgs = scrimmage_gtri_share;

CaptureTheFlagEx1::CaptureTheFlagEx1() : dive_altitude_(500)
{
}

void CaptureTheFlagEx1::init(std::map<std::string,std::string> &params)
{
    // Hardcode max_speed_ to 30 right now and fix later
    double max_speed_ = 30;
    desired_state_->vel() = max_speed_*Vector3d::UnitX();
    desired_state_->quat().set(0, 0, state_->quat().yaw());
    desired_state_->pos() = state_->pos()(2)*Vector3d::UnitZ();

    double num = (parent_.lock()->random()->rng_uniform() + 1.0) / 2.0;
    double pct_greedy = std::stod(params["percent_greedy"]);
    dist_xy_ = std::stod(params["dist_xy"]);
    dive_altitude_ = std::stod(params["dive_altitude"]);    

    if (num <= pct_greedy) {
        mode_ = GreedyShooter;
    } else {
        mode_ = DiveBomber;
    }

    fire_range_max_ = std::stod(params["fire_range_max"]);
    fire_FOV_ = sc::Angles::deg2rad(std::stod(params["fire_FOV"]));
    fire_2D_mode_ = sc::str2bool(params["fire_2D_mode"]);
    
    pub_fire_ = create_publisher("Fire");

    avoid_dist_ = std::stod(params["avoid_dist"]);
}

bool CaptureTheFlagEx1::step_autonomy(double t, double dt)
{
    // Search for closest enemy
    std::vector<sc::ID> neighbors;
    int other_team_id = parent_.lock()->id().team_id() == 1 ? 2 : 1;
    rtree_->nearest_n_neighbors(state_->pos_const(), neighbors, 1, parent_.lock()->id().id(), other_team_id);

    int hitable_id;
    if (sgs::find_hitable(state_, parent_.lock()->id(), contacts_, hitable_id,
                        fire_range_max_, fire_FOV_, fire_FOV_,
                        fire_2D_mode_)) {
        sgs::publish_fire(t, pub_fire_, network_id_, parent_.lock()->id().id(),
                          hitable_id);
    }

    //avoid hitting other things
    if(!neighbors.empty()){
        int id = neighbors.front().id();
        sc::State &tgt_state = *contacts_->at(id).state();
        Eigen::Vector3d &tgt_pos = tgt_state.pos();
        Eigen::Vector3d &self_pos = state_->pos();
        Eigen::Vector3d diff = self_pos - tgt_pos;
        if(diff.norm() < avoid_dist_){
            double heading = atan2(diff(1), diff(0));
            desired_state_->quat().set(0, 0, heading);
            desired_state_->pos() = self_pos + self_pos - tgt_pos;
            desired_state_->pos()(2) = std::abs(desired_state_->pos()(2));
            return true;
        }
    }
    std::vector<sc::ID> friends;
    rtree_->nearest_n_neighbors(state_->pos_const(), friends, 1, parent_.lock()->id().id(), parent_.lock()->id().team_id());
    if(!friends.empty()){
        int id = friends.front().id();
        sc::State &tgt_state = *contacts_->at(id).state();
        Eigen::Vector3d &tgt_pos = tgt_state.pos();
        Eigen::Vector3d &self_pos = state_->pos();
        Eigen::Vector3d diff = self_pos - tgt_pos;
        if(diff.norm() < avoid_dist_){
            double heading = atan2(diff(1), diff(0));
            desired_state_->quat().set(0, 0, heading);
            desired_state_->pos() = self_pos + self_pos - tgt_pos;
            desired_state_->pos()(2) = std::abs(desired_state_->pos()(2));
            return true;
        }
    }


    if (mode_ == GreedyShooter) {
        if (neighbors.empty()) {
            mode_ = DiveBomber;            
        } else {
            int id = neighbors.front().id();
            Vector3d &tgt_pos = contacts_->at(id).state()->pos();
            Vector3d &self_pos = state_->pos();

            double heading = atan2(tgt_pos(1) - self_pos(1),
                                   tgt_pos(0) - self_pos(0));

            desired_state_->quat().set(0, 0, heading);
            desired_state_->pos() = tgt_pos(2) * Vector3d::UnitZ();
        }
    } else if (mode_ == DiveBomber) {

        double min_dist = std::numeric_limits<double>::infinity();
        Vector3d target = Vector3d::Zero();
        Vector3d &self_pos = state_->pos();

        for (auto &kv : parent_.lock()->mp()->team_info()) {
            if (kv.first == parent_.lock()->id().team_id()) {
                continue;
            }

            for (Vector3d &base_pos : kv.second.bases) {
                double dist = (self_pos - base_pos).norm();
                if (dist < min_dist) {
                    target = base_pos;
                    min_dist = dist;
                }
            }
        }

        if (min_dist < std::numeric_limits<double>::infinity()) {
            Vector3d diff = target - self_pos;
            double heading = atan2(diff(1), diff(0));
            desired_state_->quat().set(0, 0, heading);

            double dist_xy = sqrt(pow(diff(0), 2) + pow(diff(1), 2));
            if (dist_xy > dist_xy_) {                
                desired_state_->pos()(2) = dive_altitude_;
            } else {
                desired_state_->pos()(2) = target(2);
            }            
        }
    }
    return true;
}

bool CaptureTheFlagEx1::posthumous(double t)
{
    // Accept Death
    return true;

    // // NEVER DIE! (and jump to higher altitude)
    // parent_.lock()->set_health_points(1);
    // sc::State state = *(parent_.lock()->state());
    // state.pos()[2] += 100;
    // parent_.lock()->motion()->teleport(state);
    // return false;
}
