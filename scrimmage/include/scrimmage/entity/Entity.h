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
#ifndef ENTITY_H_
#define ENTITY_H_
#include <map>
#include <unordered_map>
#include <list>
#include <vector>

#include <scrimmage/common/FileSearch.h>
#include <scrimmage/common/ID.h>
#include <scrimmage/entity/Contact.h>
#include <scrimmage/proto/Visual.pb.h>
#include <functional>
#include <memory>

namespace scrimmage_proto {
    using ContactVisualPtr = std::shared_ptr<ContactVisual>;
}

namespace scrimmage {

using Service = std::function<bool (scrimmage::MessageBasePtr, scrimmage::MessageBasePtr&)>;

typedef std::map<std::string, std::map<std::string, std::string>> AttributeMap;

class Entity : public std::enable_shared_from_this<Entity> {

 public:
    Entity();
    bool init(AttributeMap &overrides,
              std::map<std::string, std::string> &info,
              ContactMapPtr &contacts,
              MissionParsePtr mp,
              std::shared_ptr<GeographicLib::LocalCartesian> proj,
              int id, int sub_swarm_id,
              PluginManagerPtr plugin_manager,
              NetworkPtr network,
              FileSearch &file_search,
              RTreePtr &rtree);

    bool parse_visual(std::map<std::string, std::string> &info,
                      MissionParsePtr mp, FileSearch &file_search);

    StatePtr &state();
    std::vector<AutonomyPtr> &autonomies();
    MotionModelPtr &motion();
    std::vector<ControllerPtr> &controllers();

    void set_id(ID &id);
    ID &id();

    void collision();
    void hit();
    void set_health_points(int health_points);
    int health_points();
    
    bool is_alive();
    bool posthumous(double t);

    std::shared_ptr<GeographicLib::LocalCartesian> projection();

    MissionParsePtr mp();
    void set_random(RandomPtr random);
    RandomPtr random();

    void set_parameter_vector(std::vector<double> parameter_vector) { parameter_vector_= parameter_vector; }
    std::vector<double> parameter_vector() {return parameter_vector_; }
    void set_nn_path(std::string nn_path){nn_path_=nn_path;}
    std::string nn_path() {return nn_path_; }
    void set_nn_path2(std::string nn_path){nn_path2_=nn_path;}
    std::string nn_path2() {return nn_path2_; }

    Contact::Type type();

    void set_visual_changed(bool visual_changed);

    bool visual_changed();
    
    scrimmage_proto::ContactVisualPtr & contact_visual();
    
    std::unordered_map<std::string, std::list<SensablePtr>> &sensables();
    std::unordered_map<std::string, std::list<SensorPtr>> &sensors();

    void set_active(bool active);
    bool active();

    void setup_desired_state();

    std::unordered_map<std::string, Service> &services();

 protected:
    ID id_;

    scrimmage_proto::ContactVisualPtr visual_;
    
    std::vector<ControllerPtr> controllers_;
    MotionModelPtr motion_model_;
    std::vector<AutonomyPtr> autonomies_;
    MissionParsePtr mp_;

    int health_points_;

    int max_hits_;

    Contact::Type type_;

    std::shared_ptr<GeographicLib::LocalCartesian> proj_;

    RandomPtr random_;

    std::vector<double> parameter_vector_;
    std::string nn_path_;
    std::string nn_path2_;

    StatePtr state_;
    std::unordered_map<std::string, std::list<SensablePtr>> sensables_;
    std::unordered_map<std::string, std::list<SensorPtr>> sensors_;

    bool active_;
    bool visual_changed_;
    std::unordered_map<std::string, Service> services_;
};

using EntityPtr = std::shared_ptr<Entity>;
}
#endif
