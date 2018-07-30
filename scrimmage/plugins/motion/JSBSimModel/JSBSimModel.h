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
#ifndef JSBSIMMODEL_H_
#define JSBSIMMODEL_H_
#include <scrimmage/math/State.h>
#include <scrimmage/motion/MotionModel.h>
#include <scrimmage/motion/Controller.h>
#include <scrimmage/common/PID.h>
#include <scrimmage/math/Angles.h>
#include <scrimmage/entity/Entity.h>

#if ENABLE_JSBSIM==1
#include <FGFDMExec.h>
#include <models/FGAircraft.h>
#include <input_output/FGPropertyManager.h>
#include <initialization/FGInitialCondition.h>

typedef std::shared_ptr<JSBSim::FGFDMExec> FGFDMExecPtr;
#endif

class JSBSimModel : public scrimmage::MotionModel{
public:
    JSBSimModel();     

    virtual std::tuple<int,int,int> version();
     
    virtual bool init(std::map<std::string, std::string> &info,
                      std::map<std::string, std::string> &params);          
    virtual bool step(double time, double dt);          

    virtual void teleport(scrimmage::StatePtr &state);     

    class Controller : public scrimmage::Controller {
     public:
        virtual Eigen::Vector3d &u() = 0; 
    };

protected:

#if ENABLE_JSBSIM==1 
    FGFDMExecPtr exec_;
    
    JSBSim::FGPropertyNode *longitude_node_;
    JSBSim::FGPropertyNode *latitude_node_;
    JSBSim::FGPropertyNode *altitude_node_;
    
    JSBSim::FGPropertyNode *roll_node_;
    JSBSim::FGPropertyNode *pitch_node_;
    JSBSim::FGPropertyNode *yaw_node_;
    
    JSBSim::FGPropertyNode *desired_heading_node_;
    JSBSim::FGPropertyNode *desired_altitude_node_;
    JSBSim::FGPropertyNode *desired_velocity_node_;
    JSBSim::FGPropertyNode *bank_setpoint_node_;

    JSBSim::FGPropertyNode *vel_north_node_;     
    JSBSim::FGPropertyNode *vel_east_node_;     
    JSBSim::FGPropertyNode *vel_down_node_;          

    scrimmage::Angles angles_to_jsbsim_;
    scrimmage::Angles angles_from_jsbsim_;

    scrimmage::PID heading_pid_;
    double prev_desired_yaw_;
    bool heading_lag_initialized_;

    double dt_;
#endif 
     
private:     
};

#endif
