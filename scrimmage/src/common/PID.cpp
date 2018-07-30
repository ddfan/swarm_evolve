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
#include <cmath>
#include <scrimmage/math/Angles.h>
#include <scrimmage/common/PID.h>

namespace scrimmage {

    PID::PID() : kp_(0), ki_(0), kd_(0), prev_error_(0), integral_(0),
                 setpoint_(0), integral_band_(0), is_angle_(false)
    {
    }

    void PID::set_parameters(double kp, double ki, double kd)
    {
        kp_ = kp;
        ki_ = ki;
        kd_ = kd;
    }

    void PID::set_setpoint(double setpoint)
    {
        setpoint_ = setpoint;
    }

    void PID::set_integral_band(double integral_band)
    {
        integral_band_ = integral_band;
    }

    void PID::set_is_angle(bool is_angle) { is_angle_ = is_angle; }

    double PID::step(double dt, double measurement)
    {
        double error = setpoint_ - measurement;
        if (is_angle_) {
            error = Angles::angle_pi(error);
        }

        if (std::abs(error) > integral_band_) {
            integral_ = 0;
        } else {
            integral_ += error * dt;
        }

        double derivative = (error - prev_error_) / dt;
        double u = kp_*error + ki_*integral_ + kd_*derivative;
        return u;
    }
}
