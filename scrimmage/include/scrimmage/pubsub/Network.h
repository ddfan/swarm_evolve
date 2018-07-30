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
#ifndef Network_H_
#define Network_H_

#include <map>
#include <memory>
#include <list>
#include <scrimmage/fwd_decl.h>
#include <scrimmage/plugin_manager/Plugin.h>

namespace scrimmage {

class Network : public Plugin {
 public:

    Network();

    virtual void init(std::map<std::string,std::string> &params);
    virtual void distribute();
    virtual std::string name();
    virtual std::string type();

    void add_publisher(int id, PublisherPtr &pub, std::string &topic);
    void add_subscriber(int id, SubscriberPtr &sub, std::string &topic);
    void rm_publisher(int id, PublisherPtr pub, std::string &topic);
    void rm_subscriber(int id, SubscriberPtr sub, std::string &topic);
    void clear_subscriber_msgs();
    RTreePtr &rtree();

    std::map<std::string, std::map<int, std::list<SubscriberPtr>>> &sub_map();

 protected:

    // topic: <id, list of publishers on that entity>
    std::map<std::string, std::map<int, std::list<PublisherPtr>>> pub_map_; 
    std::map<std::string, std::map<int, std::list<SubscriberPtr>>> sub_map_;
    RTreePtr rtree_;

};

typedef std::shared_ptr<Network> NetworkPtr;
}

#endif // Network_H_
