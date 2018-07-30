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
#include "SimpleCarControllerHeading.h"
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>

REGISTER_PLUGIN(scrimmage::Controller,
                SimpleCarControllerHeading,
                SimpleCarControllerHeading_plugin)

namespace sc = scrimmage; 

void SimpleCarControllerHeading::init(std::map<std::string, std::string> &params) {
    double p_gain = sc::get<double>("p_gain", params, 1);
    double i_gain = sc::get<double>("i_gain", params, 1);
    double d_gain = sc::get<double>("d_gain", params, 1);
    double i_lim = sc::get<double>("i_lim", params, 1);
    pid_.set_parameters(p_gain, i_gain, d_gain);
    pid_.set_integral_band(i_lim);
}

bool SimpleCarControllerHeading::step(double t, double dt) {
    u_(0) = desired_state_->vel()(0);
    pid_.set_setpoint(desired_state_->quat().yaw());
    u_(1) = pid_.step(dt, state_->quat().yaw());
    return true;
}
