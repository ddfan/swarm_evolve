#include <iostream>
#include <memory>

#include <Eigen/Dense>
#include <boost/math/special_functions/sign.hpp>

#include <scrimmage-gtri-share/utilities/Utilities.h>
#include <scrimmage/entity/Contact.h>
#include <scrimmage/math/State.h>
#include <scrimmage/pubsub/Message.h>

#include <scrimmage/autonomy/Autonomy.h>
#include <scrimmage/plugin_manager/Plugin.h>
#include <scrimmage-gtri-share/msgs/Fire.pb.h>
#include <scrimmage/entity/Contact.h>

using Eigen::Vector2d;

namespace sc = scrimmage;

namespace scrimmage_gtri_share {

bool find_hitable(sc::StatePtr &own_state, sc::ID &own_id,
                  sc::ContactMapPtr contacts, int &hitable_id,
                  double dist_thresh, double az_thresh, double el_thresh,
                  bool use_2d)
{
    for (auto &kv : *contacts) {
        sc::Contact &cnt = kv.second;

        // Skip if own id
        if (cnt.id().id() == own_id.id()) continue;

        // Skip if on same team
        if (cnt.id().team_id() == own_id.team_id()) continue;

        // Is the enemy hitable?
        if (is_hitable(own_state, cnt.state(), dist_thresh, az_thresh,
                       el_thresh, use_2d)) {
            hitable_id = cnt.id().id();
            return true;
        }
    }
    return false;
}


bool is_hitable(sc::StatePtr &own_state, sc::StatePtr &tgt_state,
                double dist_thresh, double az_thresh, double el_thresh,
                bool use_2d)
{
    sc::State tgt = *tgt_state;
    bool fire = false;

    // Move target to same altitude if using 2D mode
    if (use_2d) {
        tgt.pos()(2) = own_state->pos()(2);
    }

    double dist = (tgt.pos() - own_state->pos()).norm();

    if (dist <= dist_thresh) {
        fire = own_state->InFieldOfView(tgt, az_thresh, el_thresh);
    }
    return fire;
}

bool publish_fire(double t, sc::PublisherPtr pub, int network_id,
                  int own_id, int tgt_id)
{
    auto msg = std::make_shared<sc::Message<Fire>>();
    msg->data.set_source_id(own_id);
    msg->data.set_target_id(tgt_id);

    msg->sender = network_id;
    msg->time = t;
    pub->publish(msg);

    return true;
}

double PropNav::update(double t, sc::State &src_state, Eigen::Vector3d &dst_pos) {

    Vector2d dst_pos2d = dst_pos.head<2>();
    Vector2d src_pos = src_state.pos().head<2>();
    Vector2d vec_to_bogy = dst_pos2d - src_pos;

    double az = atan2(vec_to_bogy(1), vec_to_bogy(0));
    double goal_ang = az;

    if (!std::isnan(last_t_) && !std::isnan(last_az_)) {
        double az_diff = sc::Angles::angle_pi(az - last_az_);
        double dt = t - last_t_;
        d_los_dt_ = az_diff / dt;
        goal_ang = src_state.quat().yaw() + (sigmoid(d_los_dt_, 0, sigmoid_w_) - 0.5) * M_PI / 2;
        goal_ang = sc::Angles::angle_pi(goal_ang);
    }
    last_az_ = az;
    last_t_ = t;
    return goal_ang;
}

double PropNav::update(double t, sc::State &src_state, sc::State &dst_state, double fire_range, double fov) {
    bool avoid;
    return update(t, src_state, dst_state, fire_range, fov, avoid);
}

double PropNav::update(double t, sc::State &src_state, sc::State &dst_state, double fire_range, double fov, bool &avoid) {

    Eigen::Vector3d pos = dst_state.pos_offset(dist_ahead_);
    double goal_ang = update(t, src_state, pos);

    Vector2d src_orient = src_state.vel().head<2>();
    Vector2d dst_pos = dst_state.pos().head<2>();
    Vector2d src_pos = src_state.pos().head<2>();
    Vector2d err = src_pos - dst_pos;

    bool src_facing_dst = err.dot(src_orient) < 0;

    if (src_facing_dst && err.norm() < fire_range && dst_state.InFieldOfView(src_state, fov, fov)) {
        avoid = true;
        double src_yaw = src_state.quat().yaw();
        if (ant_train_angle(dst_state, src_state.pos()) > 0) {
            // on right side, bank left
            goal_ang = src_yaw - M_PI / 2;
        } else {
            goal_ang = src_yaw + M_PI / 2;
        }
    } else {
        avoid = false;
    }
    return goal_ang;
}

double PointCtrl::update(double t, scrimmage::State &state, sc::State &dst_state, double fire_range, double fov, bool &avoid) {
    double goal_ang = prop_nav_.update(t, state, dst_state, fire_range, fov, avoid);
    if (std::isnan(prop_nav_.get_d_los_dt())) {
        return goal_ang;
    }
    double sign = boost::math::sign(dist_ahead_);

    dist_ahead_ +=
        gain_dlos_ * prop_nav_.get_d_los_dt() - 
        sign * gain_dist_ahead_ * dist_ahead_;

    Vector2d des_pos = dst_state.pos_offset(dist_ahead_).head<2>();
    Vector2d offset = des_pos - state.pos().head<2>();
    return atan2(offset(1), offset(0));
}


} // namespace scrimmage_gtri_share
