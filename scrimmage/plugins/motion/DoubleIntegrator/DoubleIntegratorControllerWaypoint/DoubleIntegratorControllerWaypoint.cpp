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
#include <iostream>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>
#include "DoubleIntegratorControllerWaypoint.h"
REGISTER_PLUGIN(scrimmage::Controller,
                DoubleIntegratorControllerWaypoint,
                DoubleIntegratorControllerWaypoint_plugin)

void DoubleIntegratorControllerWaypoint::init(std::map<std::string, std::string> &params) {
    std::vector<double> gain;
    if (!scrimmage::str2vec<double>(params.at("gain"), ",", gain, 2)) {
        std::cout << "warning: did not get gain properly in DoubleIntegratorControllerWaypoint" << std::endl;
    } else {
        gain_ << gain[0], gain[1];
    }
}

bool DoubleIntegratorControllerWaypoint::step(double t, double dt) {
    u_ = -gain_(0) * (state_->pos() - desired_state_->pos()) - gain_(1) * state_->vel();
    return true;
}
