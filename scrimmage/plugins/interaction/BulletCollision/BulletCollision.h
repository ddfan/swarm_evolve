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
#ifndef BULLET_COLLISION_H_
#define BULLET_COLLISION_H_

#include <scrimmage/simcontrol/EntityInteraction.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/pubsub/Subscriber.h>

#include <btBulletDynamicsCommon.h>

namespace sc = scrimmage;

class BulletCollision : public scrimmage::EntityInteraction {
public:
    BulletCollision();
    ~BulletCollision();
    virtual bool init(std::map<std::string,std::string> &mission_params,
                      std::map<std::string,std::string> &plugin_params);
    
    virtual bool step_entity_interaction(std::list<sc::EntityPtr> &ents, 
                                         double t, double dt);
    
    virtual bool collision_exists(std::list<sc::EntityPtr> &ents,
                                  Eigen::Vector3d &p);
protected:
    btCollisionConfiguration* bt_collision_configuration;
    btCollisionDispatcher* bt_dispatcher;
    btBroadphaseInterface* bt_broadphase;
    btCollisionWorld* bt_collision_world;

    
    sc::PublisherPtr team_collision_pub_;
    sc::PublisherPtr non_team_collision_pub_;

    double scene_size_;
    unsigned int max_objects_;

    std::map<int, btCollisionObject*> objects_;

    sc::SubscriberPtr sub_ent_gen_;    

    btSphereShape * sphere_shape_; // todo remove
    
private:     
};

#endif
