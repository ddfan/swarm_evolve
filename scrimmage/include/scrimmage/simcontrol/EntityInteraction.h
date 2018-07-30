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
#ifndef ENTITYINTERACTION_H_
#define ENTITYINTERACTION_H_
#include <list>
#include <map>
#include <memory>

#include <Eigen/Dense>

#include <GeographicLib/LocalCartesian.hpp>

#include <scrimmage/plugin_manager/Plugin.h>
#include <scrimmage/fwd_decl.h>

#include <scrimmage/proto/Shape.pb.h>

namespace scrimmage{

class EntityInteraction : public Plugin {

 public:
    inline virtual bool init(std::map<std::string,std::string> &mission_params,
                      std::map<std::string,std::string> &plugin_params) 
    { return true;}

    inline virtual std::string name()
    { return std::string("EntityInteraction"); }
    
    inline virtual bool step_entity_interaction(std::list<EntityPtr> &ents,
                                                double t, double dt) 
    { return false; }
    
    inline virtual std::list<std::shared_ptr<scrimmage_proto::Shape> > &shapes()
    { return shapes_; }
    
    inline virtual bool collision_exists(std::list<EntityPtr> &ents,
                                  Eigen::Vector3d &p) 
    { return false; }
    
    inline virtual void set_random(RandomPtr random)
    { random_ = random; }

    inline virtual void set_mission_parse(MissionParsePtr mp)
    { mp_ = mp; }

    inline virtual void set_projection(std::shared_ptr<GeographicLib::LocalCartesian> proj)
    { proj_ = proj;}

    inline void set_team_lookup(std::shared_ptr<std::unordered_map<int,int> > &lookup)
    { team_lookup_ = lookup; }
    
 protected:        
    std::shared_ptr<GeographicLib::LocalCartesian> proj_;
    
    std::list<std::shared_ptr<scrimmage_proto::Shape> > shapes_;
    RandomPtr random_;
    MissionParsePtr mp_;
    std::shared_ptr<std::unordered_map<int,int> > team_lookup_;
};

typedef std::shared_ptr<EntityInteraction> EntityInteractionPtr;
}
#endif
