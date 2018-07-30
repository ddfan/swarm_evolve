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
#ifndef STATE_H_
#define STATE_H_

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <memory>
#include <scrimmage/math/Quaternion.h>
#include <scrimmage/math/Angles.h>

namespace scrimmage {

class State {
 public:
    State();
    State(Eigen::Vector3d _pos, Eigen::Vector3d _vel, Quaternion _quat);

    Eigen::Vector3d &pos();
    Eigen::Vector3d &vel();
    Quaternion &quat();

    const Eigen::Vector3d &pos_const() const;
    const Eigen::Vector3d &vel_const() const;
    const Quaternion &quat_const() const;

    void set_pos(const Eigen::Vector3d &pos);
    void set_vel(const Eigen::Vector3d &vel);
    void set_quat(const Quaternion &quat);

    /*! \brief Returns true if other state is in field-of-view */
    bool InFieldOfView(State &other, double fov_width, double fov_height);

    /*! \brief convert the relative position to the local frame (the output
     * vector will point to the other state)
     */
    Eigen::Vector3d rel_pos_local_frame(Eigen::Vector3d &other);

    /*! \brief return position offset by trailing_distance in the direction of
     * velocity or orientation
     */
    Eigen::Vector3d
        pos_offset(double distance, bool offset_with_velocity = true) const;

    /*! \brief returns the vector extending forward */
    Eigen::Vector3d orient_global_frame() const;

    double rel_az(const Eigen::Vector3d &other);

 protected:
    Eigen::Vector3d pos_;
    Eigen::Vector3d vel_;
    Quaternion quat_;
};

using StatePtr = std::shared_ptr<State>;

};

#endif  // STATE_H_
