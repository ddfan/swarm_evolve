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
#ifndef SCRIMMAGE_GTRI_SHARE_UTILS_H_
#define SCRIMMAGE_GTRI_SHARE_UTILS_H_

#include <list>
#include <map>
#include <memory>
#include <unordered_map>

#include <scrimmage/math/State.h>
#include <scrimmage/entity/Contact.h>
#include <scrimmage/pubsub/Publisher.h>
#include <scrimmage/plugin_manager/Plugin.h>
#include <scrimmage/autonomy/Autonomy.h>

namespace sc = scrimmage;

namespace scrimmage_gtri_share {
    
bool find_hitable(sc::StatePtr &own_state, sc::ID &own_id,
                  sc::ContactMapPtr contacts, int &hitable_id,
                  double dist_thresh, double az_thresh, double el_thresh,
                  bool use_2d=false);

bool is_hitable(sc::StatePtr &own_state, sc::StatePtr &tgt_state,
                double dist_thresh, double az_thresh, double el_thresh,
                bool use_2d=false);

bool publish_fire(double t, sc::PublisherPtr pub, int network_id,
                  int own_id, int tgt_id);

inline double ant_train_angle(scrimmage::State &redstate, Eigen::Vector3d &bluepos) {
  // antenna train angle of the position bluepos relative to the aircraft whose redstate is given
  // (as implemented in double attack, this is the bogey's antenna train angle of blue)
    Eigen::Vector3d vec_to_pos = (bluepos - redstate.pos()).normalized();
    double yaw = redstate.quat().yaw();
    double rel_ang = atan2(vec_to_pos(1), vec_to_pos(0));
    return scrimmage::Angles::angle_pi(rel_ang - yaw);
}

inline double aspect_angle(scrimmage::StatePtr &bluestate, scrimmage::StatePtr &redstate)
{ // If blue wants red's aspect angle wrt itself, 
    // first argument should be BLUE and second argument should be RED's pos
    // Get ATA from red's perspective of blue -> this is what blue considers the Target Aspect
    // Aspect Angle is pi - TA
    // http://navyflightmanuals.tpub.com/P-825/Figure-5-P-8250028-28.htm
    // http://navyflightmanuals.tpub.com/P-825/Intercept-Geometry-Formula-Reference-60.htm
    double red_ata = ant_train_angle(*redstate,bluestate->pos());
    return scrimmage::Angles::angle_pi(M_PI - red_ata);
}
inline double yaw_rate_to_bank(double yaw_rate, double vel) {
    return atan(yaw_rate * vel / 9.81);
}

inline double bank_to_yaw_rate(double bank, double vel) {
    return -tan(bank) * 9.81 / vel;
}

inline double bank_to_radius(double bank, double vel) {
    if (vel == 0) {
        return std::numeric_limits<double>::infinity();
    } else {
        return vel / std::abs(bank_to_yaw_rate(bank, vel));
    }
}

inline double sigmoid(double x, double b, double w) {
    return 1.0 / (1 + exp(-w * x - b));
}

// Is redstate a point in bluestate's cone of vulnerability?
inline bool in_cone_vulnerability(scrimmage::StatePtr redstate, scrimmage::StatePtr bluestate, const double cov_angle_in_rad=M_PI/6)
{
    Eigen::Vector3d rel_pos = bluestate->rel_pos_local_frame(redstate->pos());
    rel_pos = -rel_pos; // so we can check rear quarter of blue instead of front quarter
    double az = atan2(rel_pos(1), rel_pos(0));
    double norm_xy = sqrt(pow(rel_pos(0), 2) + pow(rel_pos(1), 2));
    double el = atan2(rel_pos(2), norm_xy);
    return std::abs(az) < cov_angle_in_rad / 2 && std::abs(el) < cov_angle_in_rad / 2;
}

class PropNav {
 public: 
    void set_sigmoid_w(double sigmoid_w) {sigmoid_w_ = sigmoid_w;}
    double get_d_los_dt() {return d_los_dt_;}
    void set_dist_ahead(double dist_ahead) {dist_ahead_ = dist_ahead;}
    double update(double t, scrimmage::State &src_state, Eigen::Vector3d &dst_pos);
    double update(double t, scrimmage::State &src_state, scrimmage::State &dst_state, double fire_range, double fov, bool &avoid);
    double update(double t, scrimmage::State &src_state, scrimmage::State &dst_state, double fire_range, double fov);

 protected:
    double sigmoid_w_ = 10;
    double last_az_ = NAN;
    double last_t_ = NAN;
    double d_los_dt_ = NAN;
    double dist_ahead_ = 0;
};

class PointCtrl {
 public:
    PointCtrl() = default;
    double update(double t, scrimmage::State &state, scrimmage::State &des_pos, double fire_range, double fov, bool &avoid);
    double update(double t, scrimmage::State &state, scrimmage::State &des_pos, double fire_range, double fov) {
        bool avoid;
        return update(t, state, des_pos, fire_range, fov, avoid);
    }
    PropNav &prop_nav() {return prop_nav_;}
    void set_dist_ahead(double dist_ahead) {dist_ahead_ = dist_ahead;}

 protected:
    PropNav prop_nav_;
    double dist_ahead_ = 0;
    double gain_dlos_ = 1;
    double gain_dist_ahead_ = 0.5;
};

} // namespace scrimmage_gtri_share




#endif
