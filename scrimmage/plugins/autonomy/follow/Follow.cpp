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
#include <limits.h>

#include <scrimmage/entity/Entity.h>
#include <scrimmage/math/State.h>

#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/Geocentric.hpp>
#include <GeographicLib/LocalCartesian.hpp>
#include <scrimmage/plugin_manager/RegisterPlugin.h>

#include "Follow.h"

REGISTER_PLUGIN(scrimmage::Autonomy, Follow, Follow_plugin)

Follow::Follow()
{          
}

void Follow::init(std::map<std::string,std::string> &params)
{
     desired_state_->vel() = 21 * Eigen::Vector3d::UnitX();
     desired_state_->quat().set(0,0,state_->quat().yaw());
     desired_state_->pos() = state_->pos()(2) * Eigen::Vector3d::UnitZ();
}

using namespace GeographicLib;
bool Follow::step_autonomy(double t, double dt)
{                
     //// Find the closest contact
     //const GeographicLib::Geodesic& geod = Geodesic::WGS84();
     //double min_dist = 1e10;
     //bool contact_found = false;
     //std::list<scrimmage::Contact>::iterator min_champ;
     //for (std::list<scrimmage::Contact>::iterator it = contacts_.begin(); 
     //     it != contacts_.end(); it++) {
     //               
     //     if (it->id().id() != parent_.lock()->id().id()) {          
     //          //cout << "Own Lat: " << state_.pose().latitude() << endl;
     //          //cout << "Own Long: " << state_.pose().longitude() << endl;
     //          //cout << "Cnt Lat: " << it->state().pose().latitude()<< endl;
     //          //cout << "Cnt Long: " << it->state().pose().longitude()<< endl;
     //          
     //          double dist;
     //          geod.Inverse(state_.pose().latitude(), state_.pose().longitude(), 
     //                       it->state().pose().latitude(), it->state().pose().longitude(), 
     //                       dist);
     //     
     //          if (dist < min_dist) {
     //               min_dist = dist;
     //               min_champ = it;
     //               contact_found = true;
     //          }          
     //     }          
     //}
     //
     //// Head towards the closet contact if it exists
     //if (contact_found) {          
     //     Geocentric earth(Constants::WGS84_a(), Constants::WGS84_f());
     //     // Alternatively: const Geocentric& earth = Geocentric::WGS84();
     //     
     //     LocalCartesian proj(state_.pose().latitude(), 
     //                         state_.pose().longitude(), 0, earth);
     //     // Sample forward calculation
     //     double x, y, z;
     //     double h = 0;
     //     proj.Forward(min_champ->state().pose().latitude(), min_champ->state().pose().longitude(), h, x, y, z);
     //     double heading = atan2(x,y) * 180.0 / scrimmage::PI;          
     //     desired_state_.pose().set_heading(heading);          
     //}
     
     return true;
}
