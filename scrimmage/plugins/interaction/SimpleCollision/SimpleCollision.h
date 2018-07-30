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
#ifndef SIMPLE_COLLISION_H_
#define SIMPLE_COLLISION_H_

#include <scrimmage/simcontrol/EntityInteraction.h>
#include <scrimmage/entity/Entity.h>

namespace sc = scrimmage;

class SimpleCollision : public scrimmage::EntityInteraction {
public:
    SimpleCollision();
    
    virtual std::string name()
    { return std::string("SimpleCollision"); }
    
    virtual bool init(std::map<std::string,std::string> &mission_params,
                      std::map<std::string,std::string> &plugin_params);
    
    virtual bool step_entity_interaction(std::list<sc::EntityPtr> &ents, 
                                         double t, double dt);
    
    virtual bool collision_exists(std::list<sc::EntityPtr> &ents,
                                  Eigen::Vector3d &p);
protected:
    double collision_range_;
    bool startup_collisions_only_;
    bool enable_team_collisions_;
    bool enable_non_team_collisions_;
    bool init_alt_deconflict_;
    
    sc::PublisherPtr team_collision_pub_;
    sc::PublisherPtr non_team_collision_pub_;
private:     
};

#endif
