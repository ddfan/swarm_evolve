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
#ifndef JSBSimControlControllerHeadingPID_H_
#define JSBSimControlControllerHeadingPID_H_
#include "../JSBSimControl.h"

class JSBSimControlControllerHeadingPID : public JSBSimControl::Controller {
 public: 
    virtual void init(std::map<std::string, std::string> &params);
    virtual bool step(double t, double dt);
    virtual Eigen::Vector3d &u() {return u_;};
 protected:
    Eigen::Vector3d u_;
    scrimmage::Angles angles_to_jsbsim_;
    scrimmage::Angles angles_from_jsbsim_;

    scrimmage::PID heading_pid_;
    double prev_desired_yaw_;
    bool heading_lag_initialized_;

    scrimmage::PID roll_pid_;
    scrimmage::PID pitch_pid_;
    scrimmage::PID yaw_pid_;
};

#endif
