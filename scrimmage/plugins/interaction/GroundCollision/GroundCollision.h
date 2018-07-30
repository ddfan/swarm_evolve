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
#ifndef GROUND_COLLISION_H_
#define GROUND_COLLISION_H_

#include <scrimmage/simcontrol/EntityInteraction.h>
#include <scrimmage/entity/Entity.h>

namespace sc = scrimmage;

class GroundCollision : public scrimmage::EntityInteraction {
public:
    GroundCollision();

    virtual std::string name()
    { return std::string("GroundCollision"); }
    
    virtual bool init(std::map<std::string,std::string> &mission_params,
                      std::map<std::string,std::string> &plugin_params);

    virtual bool step_entity_interaction(std::list<sc::EntityPtr> &ents,
                                         double t, double dt);

    virtual bool collision_exists(std::list<sc::EntityPtr> &ents,
                                  Eigen::Vector3d &p);
protected:
    double ground_collision_z_;
    sc::PublisherPtr collision_pub_;
private:
};

#endif
