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
#ifndef QUATERNION_H_
#define QUATERNION_H_

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace scrimmage {

class Quaternion : public Eigen::Quaterniond {
 public:
    /*! \name constructors */
    ///@{
    /*! \brief set the quaternion to rotate an object around the given vector
     * at given angle
     */
    Quaternion();

    Quaternion(const Quaternion &other);

    Quaternion(const Eigen::Quaterniond &other);

    Quaternion(const double &w, const double &x,
               const double &y, const double &z);

    Quaternion(double &w, double &x,
               double &y, double &z);

    Quaternion(const Eigen::Vector3d &vector, double angle_radians);

    /*! \brief set quaternion from euler angles */
    Quaternion(double roll, double pitch, double yaw);

    void set(double w_coeff, double x_coeff, double y_coeff, double z_coeff);
        
    ///@}

    /*! \name setters */
    ///@{
    /*! \brief set the quaternion to rotate an object around the given vector
     * at given angle
     */
    void set(const Eigen::Vector3d &vector, double angle_radians);

    /*! \brief set quaternion from euler angles */
    void set(double roll, double pitch, double yaw);
    ///@}

    /*! \name utilities */
    ///@{
    /*! \brief return euler angle roll */
    double roll() const;

    /*! \brief return euler angle pitch */
    double pitch() const;

    /*! \brief return euler angle yaw */
    double yaw() const;

    /*! \brief return the angle an input vector would be rotated by this
     * quaternion
     */
    double rotation_angle() const;

    /*! \brief rotate the input vector around the \ref vector_ axis by an angle
     * given by RotationAngle(). This can also be used to convert a vector in
     * the local frame to a vector in the global frame.
     */
    Eigen::Vector3d rotate(const Eigen::Vector3d &vec) const;

    /*! \brief rotate the input vector around the \ref vector_ axis by an angle
     * given by RotationAngle(). This can also be used to convert a vector in
     * the local frame to a vector in the global frame.
     */
    Eigen::Vector3d rotate_reverse(const Eigen::Vector3d &vec) const;
    ///@}
};

} // namespace scrimmage
#endif // QUATERNION_H_
