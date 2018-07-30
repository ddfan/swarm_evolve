#include <scrimmage/math/Quaternion.h>

namespace scrimmage {

Quaternion::Quaternion() : Eigen::Quaterniond() {}

Quaternion::Quaternion(const Quaternion &other) : Eigen::Quaterniond(other) {}

Quaternion::Quaternion(const Eigen::Quaterniond &other) : Eigen::Quaterniond(other) {}

Quaternion::Quaternion(const double &w, const double &x, const double &y, const double &z)
    : Eigen::Quaterniond(w, x, y, z) {}

Quaternion::Quaternion(double &w, double &x, double &y, double &z)
    : Eigen::Quaterniond(w, x, y, z) {}

Quaternion::Quaternion(const Eigen::Vector3d &vector, double angle_radians) {
    set(vector, angle_radians);
}

Quaternion::Quaternion(double roll, double pitch, double yaw) {
    set(roll, pitch, yaw);
}

void Quaternion::set(double w_coeff, double x_coeff, double y_coeff, double z_coeff)
{
    w() = w_coeff;
    x() = x_coeff;
    y() = y_coeff;
    z() = z_coeff;
}

void Quaternion::set(const Eigen::Vector3d &vector, double angle_radians) {
    w() = cos(angle_radians / 2);
    Eigen::Vector3d unit_vec = vector.normalized() * sin(angle_radians);
    x() = unit_vec.x();
    y() = unit_vec.y();
    z() = unit_vec.z();
}

void Quaternion::set(double roll, double pitch, double yaw) {
    double sr = sin(roll / 2);
    double cr = cos(roll / 2);
    double sy = sin(yaw / 2);
    double cy = cos(yaw / 2);
    double sp = sin(pitch / 2);
    double cp = cos(pitch / 2);

    w() = cr * cp * cy + sr * sp * sy;
    x() = sr * cp * cy - cr * sp * sy;
    y() = cr * sp * cy + sr * cp * sy;
    z() = cr * cp * sy - sr * sp * cy;
}

double Quaternion::roll() const {
    return atan2(2 * (w() * x() + y() * z()),
                 1 - 2 * (pow(x(), 2) + pow(y(), 2)));
}

double Quaternion::pitch() const {
    return asin(2 * (w() * y() - z() * x()));
}

double Quaternion::yaw() const {
    return atan2(2 * (w() * z() + x() * y()),
                 1 - 2 * (pow(y(), 2) + pow(z(), 2)));
}

double Quaternion::rotation_angle() const {
    return 2 * acos(w());
}

Eigen::Vector3d Quaternion::rotate(const Eigen::Vector3d &vec) const {
    Eigen::Quaterniond pure_quat(0, vec.x(), vec.y(), vec.z());
    return (*this * pure_quat * inverse()).vec();
}

Eigen::Vector3d Quaternion::rotate_reverse(const Eigen::Vector3d &vec) const {
    Eigen::Quaterniond pure_quat(0, vec.x(), vec.y(), vec.z());
    return (inverse() * pure_quat * *this).vec();
}

}
