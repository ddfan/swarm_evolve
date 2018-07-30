// ---------------------------------------------------------------------------
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
#include "UnicycleControllerPoint.h"
REGISTER_PLUGIN(scrimmage::Controller,
                UnicycleControllerPoint,
                UnicycleControllerPoint_plugin)

void UnicycleControllerPoint::init(std::map<std::string, std::string> &params) {
    l_ = std::stod(params.at("l"));
    gain_ = std::stod(params.at("gain"));
}

bool UnicycleControllerPoint::step(double t, double dt) {
    Eigen::Vector2d des_pos = desired_state_->pos().head<2>();
    Eigen::Vector2d pos = state_->pos().head<2>();
    Eigen::Vector2d des_vel = gain_ * (des_pos - pos);

    double th = state_->quat().yaw();

    Eigen::Matrix2d M;
    M << cos(th), sin(th), -sin(th) / l_, cos(th) / l_;
    u_ = M * des_vel;
    return true;
}
