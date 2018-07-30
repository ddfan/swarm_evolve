/******************************************************************************
 * Copyright 2016, Georgia Tech Research Corporation, Atlanta, Georgia 30332
 * All Rights Reserved
 ******************************************************************************/
#include <iostream>
#include <memory>
#include <limits>

#include <boost/algorithm/string/predicate.hpp>

#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/entity/Entity.h>
#include <memory>

#include <scrimmage/math/Angles.h>
#include <scrimmage/parse/MissionParse.h>
#include <scrimmage/common/Random.h>

#include <scrimmage/pubsub/Network.h>
#include <scrimmage/pubsub/Publisher.h>
#include <scrimmage/pubsub/Subscriber.h>
#include <scrimmage/pubsub/Message.h>

#include "CaptureTheFlagInteraction.h"

#include <GeographicLib/Geocentric.hpp>
#include <GeographicLib/LocalCartesian.hpp>

#include <scrimmage/common/RTree.h>
#include <scrimmage/autonomy/Autonomy.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/common/Random.h>

#include <scrimmage/math/State.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>

#include <scrimmage/proto/ProtoConversions.h>
#include <scrimmage/proto/Shape.pb.h>

#include <scrimmage/pubsub/Message.h>
#include <scrimmage-gtri-share/msgs/Fire.pb.h>
#include <scrimmage-gtri-share/msgs/BaseCollision.pb.h>

#include <scrimmage/msgs/Event.pb.h>
namespace sm = scrimmage_msgs;

using std::cout;
using std::endl;

namespace sc = scrimmage;
namespace sp = scrimmage_proto;
namespace sgs = scrimmage_gtri_share;

REGISTER_PLUGIN(scrimmage::EntityInteraction, CaptureTheFlagInteraction,
                CaptureTheFlagInteraction_plugin)

CaptureTheFlagInteraction::CaptureTheFlagInteraction()
{
}

double rel_azimuth(Eigen::Vector3d &tgt_pos, sc::State &state) {
    Eigen::Vector3d diff = tgt_pos - state.pos();
    double az_to_tgt = atan2(diff(1), diff(0));
    double az = state.quat().yaw();
    return sc::Angles::angle_diff_rad(az, az_to_tgt);
}

bool CaptureTheFlagInteraction::create_cube(std::string cube_name,
                                            std::map<std::string,std::string> &plugin_params,
                                            Eigen::Vector3d color)
{
    std::vector<std::string> points_str;
    bool exist = sc::get_vec(cube_name+"/point", plugin_params, points_str);
    if (!exist) {
        cout << "Can't find cube:" << cube_name << endl;
        return false;
    }

    // Get the cube's points:
    std::vector<Eigen::Vector3d> points;
    for (std::string p : points_str) {
        std::vector<double> vec;
        if (sc::str2vec<double>(p, " ,", vec, 3)) {
            // Convert to x/y/z
            double x, y, z;
            proj_->Forward(vec[1], vec[0], vec[2], x, y, z);
            points.push_back(Eigen::Vector3d(x,y,z));
        }
    }

    if (points.size() != 8) {
        cout << "Invalid number of cube points: " << points.size() << endl;
        return false;
    }

    CubeInfo cube;
    cube.points = points;
    cube.compute_dots();
    cubes_[cube_name] = cube;

    if (show_cubes_) {
        const int num_faces = 6;
        const int vert_per_face = 4;
        int vert_lookup[num_faces][vert_per_face] = {
            {0,1,2,3},
            {0,1,5,4},
            {0,3,7,4},
            {6,5,4,7},
            {6,2,1,5},
            {6,7,3,2},
        };

        for (int f = 0; f < num_faces; f++) {
            sc::ShapePtr shape(new sp::Shape);
            shape->set_type(sp::Shape::Polygon);
            shape->set_opacity(0.2);
            sc::set(shape->mutable_color(), color[0], color[1], color[2]);
            shape->set_persistent(true);
            for (int r = 0; r < vert_per_face; r++) {
                sp::Vector3d * p = shape->add_point();
                sc::set(p, points[vert_lookup[f][r]]);
            }
            shapes_.push_back(shape);
        }
    }
    return true;
}

bool CaptureTheFlagInteraction::init(std::map<std::string,std::string> &mission_params,
                                     std::map<std::string,std::string> &plugin_params)
{
    base_collision_range_ = sc::get("base_collision_range", plugin_params, 0.0);

    double fire_rate = sc::get("fire_rate_max", plugin_params, -1);
    fire_period_min_ = 1 / (fire_rate == 0 ? -1 : fire_rate);

    fire_range_max_ = sc::get("fire_range_max", plugin_params, 100);
    offensive_fire_range_max_ = sc::get("offensive_fire_range_max", plugin_params, 100);
    defensive_fire_range_max_ = sc::get("defensive_range_max", plugin_params, 150);
    enable_cube_effects_ = sc::get<bool>("enable_cube_effects", plugin_params, false);

    fire_fov_width_ = sc::get("fire_fov_width", plugin_params, sc::Angles::deg2rad(10));
    fire_fov_height_ = sc::get("fire_fov_height", plugin_params, sc::Angles::deg2rad(10));

    fire_width_noise_ = sc::get("fire_width_noise", plugin_params, 0.1);
    fire_height_noise_ = sc::get("fire_height_noise", plugin_params, 0.1);
    fire_effective_range_ = sc::get("fire_effective_range", plugin_params, 10.0);

    entity_radius_ = std::stod(plugin_params.at("entity_radius"));

    fire_rounds_max_ = sc::get("fire_rounds_max", plugin_params, -1);
    std::string hit_detect_str = sc::get("hit_detection", plugin_params, (std::string)"cone");

    if (boost::iequals(hit_detect_str, "noisy_cone")) {
        hit_detection_ = HitDetection::noisy_cone;
    } else if (boost::iequals(hit_detect_str, "cone")) {
        hit_detection_ = HitDetection::cone;
    } else if (boost::iequals(hit_detect_str, "shrinking_cone")) {
        hit_detection_ = HitDetection::shrinking_cone;
    } else if (boost::iequals(hit_detect_str, "acs")) {
        hit_detection_ = HitDetection::acs;
    } else {
        std::string msg =
            "need a hit detection method in CaptureTheFlagInteraction, got \"" + hit_detect_str + "\"";
        throw std::runtime_error(msg);
    }

    show_cubes_ = sc::get<bool>("show_cubes", plugin_params, false);

    // Draw persistent shapes, if desired:
    shapes_.clear();

    if (!create_cube("blue_defense_cube", plugin_params,
                     Eigen::Vector3d(0,0,255))) return false;

    if (!create_cube("red_defense_cube", plugin_params,
                     Eigen::Vector3d(255,0,0))) return false;

    // Setup subscribers
    sub_fire_ = create_subscriber("Fire");
    sub_ent_pres_end_ = create_subscriber("EntityPresentAtEnd");

    // Setup publishers
    pub_base_coll_ = create_publisher("BaseCollision");
    pub_fire_result_ = create_publisher("FireResult");
    pub_distancefrombase_ = create_publisher("DistanceFromBase");

    return true;
}


bool CaptureTheFlagInteraction::step_entity_interaction(std::list<sc::EntityPtr> &ents,
                                                        double t, double dt)
{
    shapes_.clear();
    if (ents.empty()) {
        return true;
    }

    std::map<int, sc::EntityPtr> int_to_ent_map;
    for (sc::EntityPtr &ent : ents) {
        int_to_ent_map[ent->id().id()] = ent;
    }

    // Account for aircraft colliding with bases
    if (ents.front()->autonomies().empty()) {
        cout << "CaptureTheFlagInteraction:  cannot get autonomy" << endl;
        return true;
    }
    sc::RTreePtr &rtree = ents.front()->autonomies().front()->rtree();
    std::vector<sc::ID> neighbors;

    if (mp_) {
        for (auto &kv : mp_->team_info()) {
            int base_team_id = kv.first;

            for (Eigen::Vector3d &base_pos : kv.second.bases) {
                rtree->neighbors_in_range(base_pos, neighbors, base_collision_range_);

                for (sc::ID &id : neighbors) {
//                    std::cout << id.id() << " has hit base " << base_team_id << std::endl;
                    if(int_to_ent_map.count(id.id())){
                        sc::EntityPtr &ent = int_to_ent_map[id.id()];
                        ent->collision();

                        auto msg = std::make_shared<sc::Message<sgs::BaseCollision>>();
                        msg->data.set_entity_id(id.id());
                        msg->data.set_base_id(base_team_id);
                        if (base_team_id == id.team_id()) {
                            msg->data.set_type(sgs::BaseCollision::Friendly);
                        } else {
                            msg->data.set_type(sgs::BaseCollision::Enemy);
                        }
                        publish_immediate(t, pub_base_coll_, msg);
                    }
                }
            }
        }
    }

    for (auto msg : sub_ent_pres_end_->pop_msgs<sc::Message<sm::EntityPresentAtEnd>>()) {
        for (sc::EntityPtr &ent : ents) {
            if (ent->id().id() == msg->data.entity_id()){
                for (auto &kv : mp_->team_info()) {
                    int base_team_id = kv.first;
                    if(base_team_id != ent->id().team_id()){
                        for(Eigen::Vector3d &base_pos : kv.second.bases){
                            auto msg = std::make_shared<sc::Message<sgs::DistanceFromBase>>();
                            msg->data.set_entity_id(ent->id().id());
                            msg->data.set_base_id(base_team_id);
                            msg->data.set_distance((base_pos-ent->state()->pos()).norm());
                            publish_immediate(t, pub_distancefrombase_, msg);
                        }
                    }
                }
            }
        }
    }

    // Process fire messages
    for (auto msg : sub_fire_->pop_msgs<sc::Message<sgs::Fire>>()) {
        int src_id = msg->data.source_id();

        // Limit fire rate
        auto it_prev_fire = prev_fire_.find(src_id);
        if (fire_period_min_ > 0) {
            // Continue if a previous fire event has been recorded and the
            // minimum fire period has not passed
            if (it_prev_fire != prev_fire_.end() &&
                (t - it_prev_fire->second) < fire_period_min_) {
                continue;
            }
        }

        // limit ammunition
        auto it_num_fires = num_fires_.find(src_id);
        if (fire_rounds_max_ > 0) {
            if (it_num_fires == num_fires_.end()) {
                it_num_fires = num_fires_.insert(std::make_pair(src_id, 0)).first;
            } else if (it_num_fires->second >= fire_rounds_max_) {
                continue;
            }
        }

        // Increment the number of fires for this aircraft
        it_num_fires->second++;

        // If a previous fire time has not been recorded, insert it. If not,
        // just update the time in the iterator
        if (it_prev_fire == prev_fire_.end()) {
            prev_fire_.insert(std::make_pair(src_id, t));
        } else {
            it_prev_fire->second = t;
        }

        int target_id = msg->data.target_id();
        auto it_target_ent = int_to_ent_map.find(target_id);
        if (it_target_ent == int_to_ent_map.end()) {
            continue;
        }

        sc::EntityPtr &target_ent = it_target_ent->second;

        int src_team_id = (*team_lookup_)[src_id];
        bool is_friendly = target_ent->id().team_id() == src_team_id;

        auto it_src_ent = int_to_ent_map.find(src_id);
        if (it_src_ent == int_to_ent_map.end()) {
            std::cout << "Warning: couldn't find source id in map" << std::endl;
            continue;
        }
        sc::EntityPtr &src_ent = it_src_ent->second;

        // Add a line between the two to show the fire event
        sc::ShapePtr attempt_shape(new sp::Shape());
        attempt_shape->set_type(sp::Shape::Line);
        sc::set(attempt_shape->mutable_color(), 255,255,255);
        attempt_shape->set_opacity(1.0);

        sc::add_point(attempt_shape, src_ent->state()->pos());
        sc::add_point(attempt_shape, target_ent->state()->pos());
        shapes_.push_back(attempt_shape);

        bool is_hit = false;
        Eigen::Vector3d diff = src_ent->state()->pos() - target_ent->state()->pos();

        double range = diff.norm();

        if (hit_detection_ == HitDetection::shrinking_cone) {

            Eigen::Vector3d rel_pos = src_ent->state()->rel_pos_local_frame(target_ent->state()->pos());
            double dist = rel_pos.norm();
            double dist_xy = rel_pos.head<2>().norm();
            double az = atan2(rel_pos(1), rel_pos(0));
            double el = atan2(rel_pos(2), dist_xy);

            if (dist == 0) {
                is_hit = true;
            } else {
                double rel_mx = std::abs(atan(entity_radius_ / diff.norm()));
                is_hit = std::abs(az) < rel_mx / 2.0 && std::abs(el) < rel_mx / 2.0;
            }
        } else if (hit_detection_ == HitDetection::noisy_cone) {
            sc::State aim = *src_ent->state();   // we want a copy so no reference
            sc::Quaternion &quat = aim.quat();

            double yaw_noise = random_->rng_normal()*fire_width_noise_;
            double pitch_noise = random_->rng_normal()*fire_height_noise_;

            quat.set(quat.roll(), quat.pitch() + pitch_noise, quat.yaw() + yaw_noise);

            double fov_width = (exp(-range/fire_effective_range_) * M_PI);
            double fov_height = fov_width;

            sc::ShapePtr fire_shape(new sp::Shape);
            fire_shape->set_type(sp::Shape::Line);
            sc::set(fire_shape->mutable_color(), 0, 0, 255);
            fire_shape->set_opacity(1.0);
            sc::add_point(fire_shape, src_ent->state()->pos());
            sc::add_point(fire_shape, aim.pos());
            shapes_.push_back(fire_shape);

            if (aim.InFieldOfView(*target_ent->state(), fov_width, fov_height)) {
                is_hit = true;
            }
        } else if (hit_detection_ == HitDetection::cone) {
            if (range <= fire_range_max_ &&
                src_ent->state()->InFieldOfView(*target_ent->state(), fire_fov_width_, fire_fov_height_)) {
                is_hit = true;
            }
        } else if (hit_detection_ == HitDetection::acs) {
            // ACS detection operates in altitude deconflicted context, so need
            // to remove z-component
            diff(2) = 0;
            range = diff.norm();

            bool possible_hit =
                std::abs(src_ent->state()->rel_az(target_ent->state()->pos())) < fire_fov_width_;
            if (possible_hit) {
                if (enable_cube_effects_) {
                    double cube_range_max = -1;
                    if (cubes_["blue_defense_cube"].contains(src_ent->state()->pos())) {
                        if (src_team_id == 1) {
                            cube_range_max = defensive_fire_range_max_;
                        } else if (src_team_id == 2) {
                            cube_range_max = offensive_fire_range_max_;
                        } else {
                            cout << "CAPTURE THE FLAG WARNING: CAN'T HANDLE MORE THAN TWO TEAMS" << endl;
                        }
                    } else if (cubes_["red_defense_cube"].contains(src_ent->state()->pos())) {
                        if (src_team_id == 1) {
                            cube_range_max = offensive_fire_range_max_;
                        } else if (src_team_id == 2) {
                            cube_range_max = defensive_fire_range_max_;
                        } else {
                            cout << "CAPTURE THE FLAG WARNING: CAN'T HANDLE MORE THAN TWO TEAMS" << endl;
                        }
                    }

                    // Based on the modified fire range, is the
                    // target within range? Also, is the target in
                    // either cube?
                    if (range <= cube_range_max &&
                        (cubes_["blue_defense_cube"].contains(target_ent->state()->pos()) ||
                         cubes_["red_defense_cube"].contains(target_ent->state()->pos()))) {
                        is_hit = true;
                    }
                } else {
                    if (range <= fire_range_max_) {
                        is_hit = true;
                    }
                }
            }
        }

        if (is_hit) {
            sc::ShapePtr hit_shape(new sp::Shape);
            hit_shape->set_type(sp::Shape::Line);
            sc::set(hit_shape->mutable_color(), 255, 0, 0);
            hit_shape->set_opacity(1.0);
            hit_shape->set_ttl(10);
            sc::add_point(hit_shape, src_ent->state()->pos());
            sc::add_point(hit_shape, target_ent->state()->pos());
            shapes_.push_back(hit_shape);

            bool is_alive_before = target_ent->is_alive();
            target_ent->hit();

            auto hit_msg = std::make_shared<sc::Message<sgs::FireResult>>();
            hit_msg->data.set_source_id(src_id);
            hit_msg->data.set_target_id(target_id);
            if (is_friendly) {
                hit_msg->data.set_type(sgs::FireResult::FriendlyHit);
            } else {
                hit_msg->data.set_type(sgs::FireResult::EnemyHit);
            }
            publish_immediate(t, pub_fire_result_, hit_msg);

            if (is_alive_before && !(target_ent->is_alive())) {
                auto kill_msg = std::make_shared<sc::Message<sgs::FireResult>>();
                kill_msg->data.set_source_id(src_id);
                kill_msg->data.set_target_id(target_id);
                if (is_friendly) {
                    kill_msg->data.set_type(sgs::FireResult::FriendlyKill);
                } else {
                    kill_msg->data.set_type(sgs::FireResult::EnemyKill);
                }
                publish_immediate(t, pub_fire_result_, kill_msg);
            }
        } else {
            auto miss_msg = std::make_shared<sc::Message<sgs::FireResult>>();
            miss_msg->data.set_source_id(src_id);
            miss_msg->data.set_target_id(target_id);
            if (is_friendly) {
                miss_msg->data.set_type(sgs::FireResult::FriendlyMiss);
            } else {
                miss_msg->data.set_type(sgs::FireResult::EnemyMiss);
            }
            publish_immediate(t, pub_fire_result_, miss_msg);
        }
    }

    return true;
}

bool CaptureTheFlagInteraction::collision_exists(std::list<sc::EntityPtr> &ents,
                                                 Eigen::Vector3d &p)
{
    // This interaction plugin should be used with SimpleCollision in the
    // mission.xml file to get startup collisions
    return false;
}
