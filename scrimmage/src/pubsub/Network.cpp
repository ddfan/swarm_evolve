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
#include <memory>

#include <scrimmage/pubsub/Network.h>
#include <scrimmage/pubsub/Publisher.h>
#include <scrimmage/pubsub/Subscriber.h>
#include <scrimmage/common/RTree.h>
#include <iostream>

namespace scrimmage {

Network::Network() : rtree_(std::make_shared<RTree>()) {}

void Network::init(std::map<std::string, std::string> &params) {return;}

template <class T1, class T2> void rm(int id, T1 ptr, T2 &map, std::string &topic) {        
    auto topic_it = map.find(topic);
    if (topic_it != map.end()) {

        auto ent_it = topic_it->second.find(id);
        if (ent_it != topic_it->second.end()) {

            auto device_it = std::find(ent_it->second.begin(), ent_it->second.end(), ptr);
            if (device_it != ent_it->second.end()) {

                ent_it->second.erase(device_it);
                if (ent_it->second.empty()) {
                    topic_it->second.erase(ent_it);

                    if (topic_it->second.empty()) {
                        map.erase(topic_it);
                    }
                }
            }
        }
    }
}

template <class T1, class T2> void add(int id, T1 ptr, T2 &map, std::string topic) {
    auto it = map.find(topic);
    if (it == map.end()) {
        map[topic][id].push_back(ptr);
    } else {
        auto it2 = it->second.find(id);
        if (it2 == it->second.end()) {
            it->second[id].push_back(ptr);
        } else {
            it2->second.push_back(ptr);
        }
    }
}

void Network::rm_publisher(int id, PublisherPtr pub, std::string &topic) {
    rm(id, pub, pub_map_, topic);
}

void Network::rm_subscriber(int id, SubscriberPtr sub, std::string &topic) {
    rm(id, sub, sub_map_, topic);
}

void Network::add_publisher(int id, PublisherPtr &pub, std::string &topic) {
    add(id, pub, pub_map_, topic);
}

void Network::add_subscriber(int id, SubscriberPtr &sub, std::string &topic) {
    add(id, sub, sub_map_, topic);
}

void Network::distribute() {
    for (auto &kv : pub_map_) {
        std::string topic = kv.first;

        for (auto &kv2 : kv.second) {
            std::list<PublisherPtr> &pub_list = kv2.second;

            for (auto &pub : pub_list) {
                auto beg = pub->msg_list().begin();
                auto end = pub->msg_list().end();

                for (auto &kv3 : sub_map_[topic]) {

                    for (auto &sub : kv3.second) {
                        //std::cout << "delivering on topic " << topic << std::endl;
                        sub->msg_list().insert(sub->msg_list().end(), beg, end);
                    }

                }

                // Clear the publisher's messages after they have been delivered
                pub->msg_list().clear();
            }
        }
    }
}

std::string Network::name() { return std::string("Network"); }

std::string Network::type() { return std::string("Network"); }

void Network::clear_subscriber_msgs() {
    for (auto &kv : sub_map_) {
        for (auto &kv2 : kv.second) {
            for (auto &sub : kv2.second) {
                sub->msg_list().clear();
            }
        }
    }
}

RTreePtr &Network::rtree() {return rtree_;}

std::map<std::string, std::map<int, std::list<SubscriberPtr> > > &Network::sub_map()
{ return sub_map_; }

}
