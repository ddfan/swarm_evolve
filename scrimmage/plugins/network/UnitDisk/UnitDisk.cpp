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
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/common/ID.h>
#include <vector>
#include <scrimmage/common/RTree.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/math/State.h>
#include <scrimmage/pubsub/Publisher.h>
#include <scrimmage/pubsub/Subscriber.h>
#include <scrimmage/pubsub/Message.h>
#include <scrimmage/autonomy/Autonomy.h>
#include "UnitDisk.h"

REGISTER_PLUGIN(scrimmage::Network, UnitDisk, UnitDisk_plugin)

namespace sc = scrimmage;

void UnitDisk::init(std::map<std::string,std::string> &params) {
    range_ = std::stod(params.at("range"));
}

void UnitDisk::distribute() {

    for (auto &kv : pub_map_) {
        std::string topic = kv.first;

        for (auto &kv2 : kv.second) {
            std::list<sc::PublisherPtr> &pub_list = kv2.second;

            for (sc::PublisherPtr &pub : pub_list) {
                if (pub->msg_list().empty()) {
                    continue;
                }

                auto beg = pub->msg_list().begin();
                auto end = pub->msg_list().end();

                Eigen::Vector3d &pos = pub->plugin()->parent()->state()->pos();
                std::vector<sc::ID> neigh;
                rtree_->neighbors_in_range(pos, neigh, range_);
                
                std::map<int, std::list<sc::SubscriberPtr>> &m = sub_map_[topic];
                for (sc::ID &id : neigh) {
                    auto it = m.find(id.id());
                    if (it != m.end()) {
                        std::list<sc::SubscriberPtr> sub_list = it->second;
                        for (sc::SubscriberPtr &sub : sub_list) {
                            sub->msg_list().insert(sub->msg_list().end(), beg, end);
                        }
                    }
                }

                // Clear the publisher's messages after they have been delivered
                pub->msg_list().clear();
            }
        }
    }
}
