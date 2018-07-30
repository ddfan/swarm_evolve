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
#include <scrimmage/plugin_manager/RegisterPlugin.h>

#include <scrimmage/math/Angles.h>

#include "WayPoint.h"

REGISTER_PLUGIN(scrimmage::Autonomy, WayPoint, WayPoint_plugin)

WayPoint::WayPoint()
{     
}

void WayPoint::init(std::map<std::string,std::string> &params)
{     
    desired_state_->vel() = 21*Eigen::Vector3d::UnitX();
    desired_state_->quat().set(0,0,state_->quat().yaw());
    desired_state_->pos() = state_->pos()(2)*Eigen::Vector3d::UnitZ();          
}

bool WayPoint::step_autonomy(double t, double dt)
{
    //cout << "---------------------------" << endl;
    //cout << "Desired Altitude: " << desired_state_.position_.z_ << endl;
    //cout << "Actual Altitude: " << state_.position_.z_ << endl;
    //
    if (t < 4) {
    } else if (t < 20) {                   
        desired_state_->quat().set(0,0,scrimmage::Angles::deg2rad(90));
    } else if (t < 50) {
    }  
     
    //
    ////if (t < 10) {
    ////     desired_state_.pose().set_altitude(4);          
    ////} else if (t < 50) {
    ////     desired_state_.pose().set_altitude(8);
    ////     desired_state_.pose().set_heading(90);
    ////} else if (t < 100) {
    ////     desired_state_.pose().set_altitude(6);
    ////     desired_state_.pose().set_heading(270);
    ////}
    //
    ////if (t < 10) {
    ////     desired_state_.pose().set_heading(0);
    ////} else if (t < 20) {
    ////     desired_state_.pose().set_heading(135);
    ////} else {
    ////     desired_state_.pose().set_heading(0);
    ////}
    return true;
}
