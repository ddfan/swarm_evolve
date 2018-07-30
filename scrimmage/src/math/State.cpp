#include <scrimmage/math/State.h>

namespace scrimmage {

State::State() {}

State::State(Eigen::Vector3d _pos, Eigen::Vector3d _vel, Quaternion _quat) :
    pos_(_pos), vel_(_vel), quat_(_quat) {}

Eigen::Vector3d &State::pos() {return pos_;}

Eigen::Vector3d &State::vel() {return vel_;}

Quaternion &State::quat() {return quat_;}

const Eigen::Vector3d &State::pos_const() const {return pos_;}

const Eigen::Vector3d &State::vel_const() const {return vel_;}

const Quaternion &State::quat_const() const {return quat_;}

void State::set_pos(const Eigen::Vector3d &pos) {pos_ = pos;}

void State::set_vel(const Eigen::Vector3d &vel) {vel_ = vel;}

void State::set_quat(const Quaternion &quat) {quat_ = quat;}

bool State::InFieldOfView(State &other, double fov_width, double fov_height) {
    Eigen::Vector3d rel_pos = this->rel_pos_local_frame(other.pos());
    double az = atan2(rel_pos(1), rel_pos(0));
    double norm_xy = sqrt(pow(rel_pos(0), 2) + pow(rel_pos(1), 2));
    double el = atan2(rel_pos(2), norm_xy);
    return std::abs(az) < fov_width / 2 && std::abs(el) < fov_height / 2;
}

Eigen::Vector3d State::rel_pos_local_frame(Eigen::Vector3d &other) {
    return quat_.rotate_reverse(other - pos_);
}

Eigen::Vector3d State::pos_offset(double distance, bool offset_with_velocity) const {
    if (offset_with_velocity) {
        return pos_ + vel_.normalized() * distance;
    } else {
        return pos_ + orient_global_frame() * distance;
    }
}

Eigen::Vector3d State::orient_global_frame() const {
    return quat_.rotate_reverse(Eigen::Vector3d::UnitX());
}

double State::rel_az(const Eigen::Vector3d &other) {
    Eigen::Vector3d diff = other - pos_;
    double az = atan2(diff(1), diff(0));
    return Angles::angle_diff_rad(az, quat_.yaw());
}

}
