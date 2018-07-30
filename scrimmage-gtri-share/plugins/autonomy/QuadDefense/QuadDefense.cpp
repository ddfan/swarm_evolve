/******************************************************************************
 * Copyright 2016, Georgia Tech Research Corporation, Atlanta, Georgia 30332
 * All Rights Reserved
 ******************************************************************************/
#include <iostream>

#include <scrimmage/entity/Entity.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/math/Angles.h>
#include <scrimmage/common/Random.h>
#include <scrimmage/common/RTree.h>
#include <scrimmage/parse/MissionParse.h>
#include <scrimmage/math/State.h>
#include <scrimmage-gtri-share/utilities/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>
#include "QuadDefense.h"

using std::cout;
using std::endl;

REGISTER_PLUGIN(scrimmage::Autonomy, QuadDefense, QuadDefense_plugin)

namespace sc = scrimmage;
namespace sgs = scrimmage_gtri_share;

QuadDefense::QuadDefense()
{     
}

void QuadDefense::init(std::map<std::string,std::string> &params)
{
    desired_state_->vel() = Eigen::Vector3d::Zero();
    desired_state_->quat().set(0,0,state_->quat().yaw());
    desired_state_->pos() = state_->pos()(2)*Eigen::Vector3d::UnitZ();

    double x_var=std::stod(params["x_var"]);
    double y_var=std::stod(params["y_var"]);
    double z_var=std::stod(params["z_var"]);

    x_offset_ = parent_.lock()->random()->rng_uniform() * x_var;
    y_offset_ = parent_.lock()->random()->rng_uniform() * y_var;
    z_offset_ = (parent_.lock()->random()->rng_uniform() + 1) * z_var;

    fire_range_max_ = std::stod(params["fire_range_max"]);
    fire_FOV_ = sc::Angles::deg2rad(std::stod(params["fire_FOV"]));
    fire_2D_mode_ = sc::str2bool(params["fire_2D_mode"]);

    pub_fire_ = create_publisher("Fire");
    avoid_dist_ = std::stod(params["avoid_dist"]);

    sub_oneteam_ = create_subscriber("OneTeamPresent");

    in_position_ = false;
}

bool QuadDefense::step_autonomy(double t, double dt)
{
    //if quad team is the only team left, send them crashing into the ground to end the scenario
    if(sub_oneteam_->msgs().size()){
        desired_state_->pos()(2)=-100;
        return true;
    }

    std::vector<sc::ID> neighbors; 
    int other_team_id = parent_.lock()->id().team_id() == 1 ? 2 : 1;
    rtree_->nearest_n_neighbors(state_->pos(), neighbors, 1, parent_.lock()->id().id(), other_team_id);

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


    if (!in_position_) {

        double min_dist = std::numeric_limits<double>::infinity();
        bool min_dist_found = false;
        Eigen::Vector3d own_base(0,0,0);

        // Find closest own base and move towards it with random offsets for x,y,z
        for (std::map<int,scrimmage::TeamInfo>::iterator it = parent_.lock()->mp()->team_info().begin();
             it != parent_.lock()->mp()->team_info().end(); it++) {
 
            // Skip if this isn't a friendly base
            if (it->first != parent_.lock()->id().team_id()) {
                continue;
            }
 
            // Search over all own bases
            for (std::list<Eigen::Vector3d>::iterator it2 = it->second.bases.begin();
                 it2 != it->second.bases.end(); it2++) {
        
                // Calculate distance to own base
                double dist = (state_->pos() - *it2).norm();
                if (dist < min_dist) {
                    own_base = *it2;
                    min_dist = dist;                    
                    min_dist_found = true;               
                }
            }                    
        }     

        if (min_dist_found) {                    
            own_base(0) += x_offset_;
            own_base(1) += y_offset_;                    
             
            if (!neighbors.empty()) {
//                int id = neighbors.front().id();
//                Eigen::Vector3d diff = (contacts_->at(id).state()->pos() - own_base).normalized();
                own_base =  own_base;
            }
     
            // If a own_base has been found, go towards it
            Eigen::Vector3d diff = own_base - state_->pos();
            double heading = atan2(diff(1), diff(0));
            desired_state_->quat().set(0, 0, heading);                         

            desired_state_->pos() << own_base(0), own_base(1), (10 + z_offset_);

            in_position_ = (diff.head<2>().norm() < 20);
        }

    } else if (!neighbors.empty()) {
         
        int id = neighbors.front().id();
        sc::State &tgt_state = *contacts_->at(id).state();
        Eigen::Vector3d &tgt_pos = tgt_state.pos();
        Eigen::Vector3d &self_pos = state_->pos();
        Eigen::Vector3d diff = tgt_pos - self_pos;
          
        double heading = atan2(diff(1), diff(0));
     
        desired_state_->quat().set(0, 0, heading);          
          
        desired_state_->pos() << self_pos(0), self_pos(1), std::max((double)tgt_pos(2),10.0);

    } else {
        in_position_ = false;
    }
    
    return true;
}
