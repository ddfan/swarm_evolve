/*
 * ---------------------------------------------------------------------------
 * @section LICENSE
 *  
 * Copyright (c) 2016 Georgia Tech Research Institute (GTRI) 
 *               All Rights Reserved
 *  
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 * ---------------------------------------------------------------------------
 * @file filename.ext
 * @author Kevin DeMarco <kevin.demarco@gtri.gatech.edu> 
 * @author Eric Squires <eric.squires@gtri.gatech.edu>
 * @version 1.0
 * ---------------------------------------------------------------------------
 * @brief A brief description.
 * 
 * @section DESCRIPTION
 * A long description.
 * ---------------------------------------------------------------------------
 */
#include <memory>

#include <scrimmage/entity/External.h>
#include <GeographicLib/Geocentric.hpp>
#include <GeographicLib/LocalCartesian.hpp>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/autonomy/Autonomy.h>
#include <scrimmage/plugin_manager/PluginManager.h>
#include <scrimmage/pubsub/Network.h>
#include <scrimmage/common/Random.h>
#include <scrimmage/entity/Contact.h>
#include <scrimmage/common/RTree.h>

namespace scrimmage {

External::External() : plugin_manager_(std::make_shared<PluginManager>()) {}

NetworkPtr &External::network() {return network_;}

EntityPtr &External::entity() {return entity_;}

FileSearch &External::file_search() {return file_search_;}

PluginManagerPtr &External::plugin_manager() {return plugin_manager_;}

int External::get_max_entities() {return max_entities_;}

void External::set_max_entities(int max_entities) {max_entities_ = max_entities;}

bool External::create_entity(ID id, std::map<std::string, std::string> &info) {
    // info needs the following keys:
    // latitude, longitude, x, y, z, heading, autonomy_name, controller
    entity_ = std::make_shared<Entity>();

    double lat = std::stod(info["latitude"]);
    double lon = std::stod(info["longitude"]);

    auto proj = std::make_shared<GeographicLib::LocalCartesian>(
        lat, lon, 0, GeographicLib::Geocentric::WGS84()
    );

    info["team_id"] = std::to_string(id.team_id());

    network_ = std::make_shared<Network>();

    ContactMapPtr contacts = std::make_shared<ContactMap>();
    std::shared_ptr<RTree> rtree = std::make_shared<RTree>();
    rtree->init(max_entities_);

    AttributeMap overrides;
    bool success = 
        entity_->init(
            overrides, info, contacts, nullptr, proj, id.id(), id.sub_swarm_id(),
            plugin_manager_, network_, file_search_, rtree);

    if (!success) {
        return false;
    }

    RandomPtr random = std::make_shared<Random>();
    entity_->set_random(random);    

    return true;
}

} // namespace scrimmage
