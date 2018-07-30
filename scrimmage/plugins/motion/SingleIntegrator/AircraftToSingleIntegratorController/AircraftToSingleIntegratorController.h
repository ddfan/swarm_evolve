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
#ifndef AircraftToSingleIntegratorController_H_
#define AircraftToSingleIntegratorController_H_
#include "../SingleIntegrator.h"

class AircraftToSingleIntegratorController : public SingleIntegrator::Controller {
 public: 
    virtual void init(std::map<std::string, std::string> &params) {}
    virtual bool step(double t, double dt) {
        double desired_heading = desired_state_->quat().yaw();
        double desired_alt = desired_state_->pos()(2);
        double desired_speed = desired_state_->vel()(0);

        // Convert desired speed, pitch, and heading into velocity
        u_[0] = desired_speed * cos(desired_heading);
        u_[1] = desired_speed * sin(desired_heading);
        u_[2] = desired_alt - state_->pos()(2);
    
        return true;
    }
    virtual Eigen::Vector3d &u() {return u_;};
 protected:
    Eigen::Vector3d u_;
};

#endif
