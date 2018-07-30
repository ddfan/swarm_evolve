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
#include <scrimmage/entity/Entity.h>
#include <scrimmage/math/State.h>
#include <scrimmage/math/Angles.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>

#include "QuadTest.h"

REGISTER_PLUGIN(scrimmage::Autonomy, QuadTest, QuadTest_plugin)

QuadTest::QuadTest()
{     
}

void QuadTest::init(std::map<std::string,std::string> &params)
{
     desired_state_->vel() = Eigen::Vector3d::Zero();
     desired_state_->quat().set(0,0,state_->quat().yaw());
     desired_state_->pos() = state_->pos()(2)*Eigen::Vector3d::UnitZ();

     if (params.count("take_off_time") > 0) {
          take_off_time_ = std::stod(params["take_off_time"]);
     } else {
          take_off_time_ = 0;
     }
}

bool QuadTest::step_autonomy(double t, double dt)
{
          

     if (t >= take_off_time_) {
          desired_state_->vel() = 15*Eigen::Vector3d::UnitX();
          desired_state_->quat().set(0,0,scrimmage::Angles::deg2rad(90));
          desired_state_->pos() = 10*Eigen::Vector3d::UnitZ();
     }
     
     return true;
}
