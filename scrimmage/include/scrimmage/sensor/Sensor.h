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
#ifndef SENSOR_H_
#define SENSOR_H_

#include <map>
#include <memory>

#include <scrimmage/plugin_manager/Plugin.h>
#include <scrimmage/fwd_decl.h>

namespace scrimmage {

class Sensor : public Plugin {
 public:
    virtual inline void init(std::map<std::string,std::string> &params) {return;}
    virtual inline bool sense(double t, double dt) {return true;}
    inline ContactMapPtr &contacts() {return contacts_;}
    inline RTreePtr &rtree() {return rtree_;}

 protected: 
    ContactMapPtr contacts_;
    RTreePtr rtree_;
};

using SensorPtr = std::shared_ptr<Sensor>;
}  // namespace scrimmage

#endif // SENSOR
