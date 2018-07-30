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
#ifndef PLUGIN_H_
#define PLUGIN_H_
#include <memory>
#include <map>

#include <scrimmage/fwd_decl.h>

namespace scrimmage {

class Plugin : public std::enable_shared_from_this<Plugin> {
public:
    Plugin();
    virtual ~Plugin();
    
    virtual std::string name();
    virtual std::string type();

    virtual void set_parent(EntityPtr parent);
    virtual EntityPtr parent();
    
    NetworkPtr &network();
    void set_network(NetworkPtr network);

    std::map<std::string, PublisherPtr> &pubs();
    void set_pubs(std::map<std::string, PublisherPtr> pubs);

    std::map<std::string, SubscriberPtr> &subs();
    void set_subs(std::map<std::string, SubscriberPtr> subs);

    // networking
    PublisherPtr create_publisher(std::string topic);
    void stop_publishing(PublisherPtr &pub);
    SubscriberPtr create_subscriber(std::string topic);
    void stop_subscribing(SubscriberPtr &sub);

    void publish(double t, PublisherPtr pub, MessageBasePtr msg);
    void publish_immediate(double t, PublisherPtr pub, MessageBasePtr msg);
    
    void clear_subscribers();
    int get_network_id() {return network_id_;}

protected:    
    int network_id_;
    static int plugin_count_;
    std::weak_ptr<Entity> parent_;
    NetworkPtr network_;    
    std::map<std::string, PublisherPtr> pubs_;
    std::map<std::string, SubscriberPtr> subs_;    
};

using PluginPtr = std::shared_ptr<Plugin>;
}
#endif
