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
#include "CaptureTheFlagLearn.h"

using std::cout;
using std::endl;
using Eigen::Vector3d;

REGISTER_PLUGIN(scrimmage::Autonomy, CaptureTheFlagLearn, CaptureTheFlagLearn_plugin)

namespace sc = scrimmage;
namespace sgs = scrimmage_gtri_share;
using namespace tiny_dnn;
using namespace tiny_dnn::layers;

CaptureTheFlagLearn::CaptureTheFlagLearn() : dive_altitude_(500)
{
}

void CaptureTheFlagLearn::init(std::map<std::string,std::string> &params)
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
    avoid_ground_height_ = std::stod(params["avoid_ground_height"]);

    sigma_ = parent_.lock()->parameter_vector()[0];
    n_friends_ = parent_.lock()->parameter_vector()[1];
    n_enemies_ = parent_.lock()->parameter_vector()[2];

    //peturb weights of neural network with random seed
    if (parent_.lock()->id().team_id() == 1){
        std::string nn_path = parent_.lock()->nn_path();
        policy_network.load(nn_path);
        policy_network.perturb_weights(std::stoi(parent_.lock()->mp()->params()["seed"]), sigma_);
    }else{
        std::string nn_path = parent_.lock()->nn_path2();
        policy_network.load(nn_path);
        policy_network.perturb_weights(std::stoi(parent_.lock()->mp()->params()["seed"])+1, sigma_);
    }

}

bool CaptureTheFlagLearn::step_autonomy(double t, double dt)
{
    // Search for closest enemy
    std::vector<sc::ID> enemy_neighbors;
    int other_team_id = parent_.lock()->id().team_id() == 1 ? 2 : 1;
    rtree_->nearest_n_neighbors(state_->pos_const(), enemy_neighbors, n_enemies_, parent_.lock()->id().id(), other_team_id);

    //shoot at enemies
    int hitable_id;
    if (sgs::find_hitable(state_, parent_.lock()->id(), contacts_, hitable_id,
                        fire_range_max_, fire_FOV_, fire_FOV_,
                        fire_2D_mode_)) {
        sgs::publish_fire(t, pub_fire_, network_id_, parent_.lock()->id().id(),
                          hitable_id);
    }

    //avoid hitting the ground
    if(state_->pos()(2) < avoid_ground_height_){
        desired_state_->pos()(2) = 2.0*avoid_ground_height_;
        return true;
    }


    //avoid hitting other team members or enemies
    if(!enemy_neighbors.empty()){
        int id = enemy_neighbors.front().id();
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
    std::vector<sc::ID> my_neighbors;
    rtree_->nearest_n_neighbors(state_->pos_const(), my_neighbors, n_friends_, parent_.lock()->id().id(), parent_.lock()->id().team_id());
    if(!my_neighbors.empty()){
        int id = my_neighbors.front().id();
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

    //nn policy
    vec_t in(policy_network.in_data_size(),0.0);
    size_t input_idx=0;

    double pos_scale=100.0;
    double angle_scale=M_PI;
    double vel_scale=10.0;

    //mirror coordinates so that behavior of team 1 and team 2 are the same
    double sign_change=1;
    double shift_angle=0;
    if (parent_.lock()->id().team_id() == 2){
        sign_change=-1;
        shift_angle=M_PI;
    }

    //add self states to nn input
    in[input_idx++]=state_->pos()(0)/pos_scale*sign_change;
    in[input_idx++]=state_->pos()(1)/pos_scale*sign_change;
    in[input_idx++]=state_->pos()(2)/pos_scale;
    in[input_idx++]=state_->quat().roll()/angle_scale;
    in[input_idx++]=state_->quat().pitch()/angle_scale;
    in[input_idx++]=state_->quat().yaw()/angle_scale+shift_angle;
    in[input_idx++]=state_->vel()(0)/vel_scale*sign_change;
    in[input_idx++]=state_->vel()(1)/vel_scale*sign_change;
    in[input_idx++]=state_->vel()(2)/vel_scale;

    //add relative home and enemy base headings to input
    Vector3d home_base = Vector3d::Zero();
    Vector3d enemy_base = Vector3d::Zero();
    for (auto &kv : parent_.lock()->mp()->team_info()) {
        if (kv.first == parent_.lock()->id().team_id())
            home_base = kv.second.bases.front() - state_->pos();
        else
            enemy_base = kv.second.bases.front() - state_->pos();
    }
    in[input_idx++]=home_base[0]/pos_scale;
    in[input_idx++]=home_base[1]/pos_scale;
    in[input_idx++]=home_base[2]/pos_scale;
    in[input_idx++]=std::sqrt(home_base.norm())/pos_scale;
    in[input_idx++]=enemy_base[0]/pos_scale;
    in[input_idx++]=enemy_base[1]/pos_scale;
    in[input_idx++]=enemy_base[2]/pos_scale;
    in[input_idx++]=std::sqrt(enemy_base.norm())/pos_scale;

    //add n friendly neighbor states
    for (size_t i = 0; i<n_friends_; i++){
        if (i<my_neighbors.size()){
            int id = my_neighbors[i].id();
            in[input_idx++]=(contacts_->at(id).state()->pos()(0) - state_->pos()(0))/pos_scale*sign_change;
            in[input_idx++]=(contacts_->at(id).state()->pos()(1) - state_->pos()(1))/pos_scale*sign_change;
            in[input_idx++]=(contacts_->at(id).state()->pos()(2) - state_->pos()(2))/pos_scale;
            in[input_idx++]=(contacts_->at(id).state()->pos() - state_->pos()).norm()/pos_scale;
            in[input_idx++]=contacts_->at(id).state()->quat().roll()/angle_scale;
            in[input_idx++]=contacts_->at(id).state()->quat().pitch()/angle_scale;
            in[input_idx++]=contacts_->at(id).state()->quat().yaw()/angle_scale+shift_angle;
            in[input_idx++]=contacts_->at(id).state()->vel()(0)/vel_scale*sign_change;
            in[input_idx++]=contacts_->at(id).state()->vel()(1)/vel_scale*sign_change;
            in[input_idx++]=contacts_->at(id).state()->vel()(2)/vel_scale;
        }else{
            input_idx+=10;
        }
    }

    //add n enemy neighbors
    for (size_t i = 0; i<n_enemies_; i++){
        if (i<enemy_neighbors.size()){
            int id = enemy_neighbors[i].id();
            in[input_idx++]=(contacts_->at(id).state()->pos()(0) - state_->pos()(0))/pos_scale*sign_change;
            in[input_idx++]=(contacts_->at(id).state()->pos()(1) - state_->pos()(1))/pos_scale*sign_change;
            in[input_idx++]=(contacts_->at(id).state()->pos()(2) - state_->pos()(2))/pos_scale;
            in[input_idx++]=(contacts_->at(id).state()->pos() - state_->pos()).norm()/pos_scale;
            in[input_idx++]=contacts_->at(id).state()->vel()(0)/vel_scale*sign_change;
            in[input_idx++]=contacts_->at(id).state()->vel()(1)/vel_scale*sign_change;
            in[input_idx++]=contacts_->at(id).state()->vel()(2)/vel_scale;
        }else{
            input_idx+=7;
        }
    }
    // pass input through neural network
    auto res = policy_network.predict(in);

    desired_state_->quat().set(0,0,res[0]*angle_scale+shift_angle); //set heading
    desired_state_->pos() = (state_->pos()(2) + res[1]*pos_scale) * Vector3d::UnitZ(); //set altitude
    desired_state_->vel()(0) = state_->vel().norm() + res[2]*vel_scale; //set velocity


//    //decide on greedy shooter or dive bomber behavior
//    if (res[0]>0)
//        mode_ = GreedyShooter;
//    else
//        mode_ = DiveBomber;

//    if (mode_ == GreedyShooter) {
//        if (enemy_neighbors.empty()) {
//            mode_ = DiveBomber;
//        } else {
//            int id = enemy_neighbors.front().id();
//            Vector3d &tgt_pos = contacts_->at(id).state()->pos();
//            Vector3d &self_pos = state_->pos();

//            double heading = atan2(tgt_pos(1) - self_pos(1),
//                                   tgt_pos(0) - self_pos(0));

//            desired_state_->quat().set(0, 0, heading);
//            desired_state_->pos() = tgt_pos(2) * Vector3d::UnitZ();
//        }
//    } else if (mode_ == DiveBomber) {

//        double min_dist = std::numeric_limits<double>::infinity();
//        Vector3d target = Vector3d::Zero();
//        Vector3d &self_pos = state_->pos();

//        for (auto &kv : parent_.lock()->mp()->team_info()) {
//            if (kv.first == parent_.lock()->id().team_id()) {
//                continue;
//            }

//            for (Vector3d &base_pos : kv.second.bases) {
//                double dist = (self_pos - base_pos).norm();
//                if (dist < min_dist) {
//                    target = base_pos;
//                    min_dist = dist;
//                }
//            }
//        }

//        if (min_dist < std::numeric_limits<double>::infinity()) {
//            Vector3d diff = target - self_pos;
//            double heading = atan2(diff(1), diff(0));
//            desired_state_->quat().set(0, 0, heading);

//            double dist_xy = sqrt(pow(diff(0), 2) + pow(diff(1), 2));
//            if (dist_xy > dist_xy_) {
//                desired_state_->pos()(2) = dive_altitude_;
//            } else {
//                desired_state_->pos()(2) = target(2);
//            }
//        }
//    }

    return true;
}

bool CaptureTheFlagLearn::posthumous(double t)
{
//    policy_network.cleanup();

    // Accept Death
    return true;

    // // NEVER DIE! (and jump to higher altitude)
    // parent_.lock()->set_health_points(1);
    // sc::State state = *(parent_.lock()->state());
    // state.pos()[2] += 100;
    // parent_.lock()->motion()->teleport(state);
    // return false;
}
