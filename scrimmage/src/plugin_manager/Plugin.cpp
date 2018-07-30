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

#include <scrimmage/plugin_manager/Plugin.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/pubsub/Network.h>
#include <scrimmage/pubsub/Publisher.h>
#include <scrimmage/pubsub/Subscriber.h>

namespace scrimmage {

int Plugin::plugin_count_ = 0;

Plugin::Plugin() : network_id_(plugin_count_++)
{
}

Plugin::~Plugin() {
    clear_subscribers();
}

std::string Plugin::name() { return std::string("Plugin"); }

std::string Plugin::type() { return std::string("Plugin"); }

void Plugin::set_parent(EntityPtr parent) {parent_ = parent;}

EntityPtr Plugin::parent() { return parent_.lock(); }

NetworkPtr &Plugin::network() {return network_;}

void Plugin::set_network(NetworkPtr network) {network_ = network;}

std::map<std::string, PublisherPtr> &Plugin::pubs() {return pubs_;}

void Plugin::set_pubs(std::map<std::string, PublisherPtr> pubs) {pubs_ = pubs;}

std::map<std::string, SubscriberPtr> &Plugin::subs() {return subs_;}

void Plugin::set_subs(std::map<std::string, SubscriberPtr> subs) {subs_ = subs;}

PublisherPtr Plugin::create_publisher(std::string topic)
{
    PublisherPtr pub = std::make_shared<Publisher>();
    pub->set_topic(topic);
    network_->add_publisher(network_id_, pub, topic);
    pub->plugin() = shared_from_this();
    pubs_[topic] = pub;
    return pub;
}

void Plugin::stop_publishing(PublisherPtr &pub)
{
    std::string topic = pub->get_topic();
    network_->rm_publisher(network_id_, pub, topic);
    
    auto it = pubs_.find(topic);
    if (it != pubs_.end()) {
        pubs_.erase(it);
    }

    pub->msg_list().clear();
    pub->set_topic("");
}

SubscriberPtr Plugin::create_subscriber(std::string topic)
{
    SubscriberPtr sub = std::make_shared<Subscriber>();
    sub->set_topic(topic);
    network_->add_subscriber(network_id_, sub, topic);
    subs_[topic] = sub;
    return sub;
}

void Plugin::stop_subscribing(SubscriberPtr &sub)
{
    std::string topic = sub->get_topic();
    network_->rm_subscriber(network_id_, sub, topic);

    auto it = subs_.find(topic);
    if (it != subs_.end()) {
        subs_.erase(it);
    }

    sub->msg_list().clear();
    sub->set_topic("");
}

void Plugin::publish(double t, PublisherPtr pub, MessageBasePtr msg)
{
    msg->sender = network_id_;
    msg->time = t;
    pub->publish(msg);       
}

void Plugin::publish_immediate(double t, PublisherPtr pub, MessageBasePtr msg)
{
    msg->sender = network_id_;
    msg->time = t;
    for (auto &kv : network_->sub_map()[pub->get_topic()]) {
        for (auto &sub : kv.second) {
            //std::cout << "delivering on topic " << topic << std::endl;
            sub->msg_list().push_back(msg);
        }
    }
}    
    
void Plugin::clear_subscribers()
{
    for (auto &kv : subs_) {
        kv.second->msg_list().clear();
    }
}

} // namespace scrimmage
