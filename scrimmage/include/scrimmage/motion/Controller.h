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
#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include <memory>
#include <scrimmage/fwd_decl.h>
#include <scrimmage/plugin_manager/Plugin.h>

namespace scrimmage {

class Controller : public Plugin {
 public:
    virtual void init(std::map<std::string, std::string> &params) = 0;
    virtual bool step(double t, double dt) = 0;
    inline void set_state(StatePtr &state) {state_ = state;}
    inline void set_desired_state(StatePtr &desired_state) {desired_state_ = desired_state;}
    
 protected:
    StatePtr state_;
    StatePtr desired_state_;
};

using ControllerPtr = std::shared_ptr<Controller>;

}
#endif // CONTROLLER_H_
