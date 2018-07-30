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
#ifndef Network_Device_H_
#define Network_Device_H_

#include <iostream>
#include <list>
#include <memory>
#include <scrimmage/fwd_decl.h>
#include <scrimmage/pubsub/MessageBase.h>
#include <type_traits>
#include <google/protobuf/message.h>

namespace scrimmage {

template <class T>
inline typename std::enable_if<!std::is_base_of<google::protobuf::Message, T>::value, bool>::type deserialize(T &proto, std::string &serialized_data) {
    return false;
}

template <class T>
typename std::enable_if<std::is_base_of<google::protobuf::Message, T>::value, bool>::type deserialize(T &proto, std::string &serialized_data) {
    return proto.ParseFromString(serialized_data);
}

template <class T> bool construct_msg(MessageBasePtr msg, T msg_cast) {
    return msg_cast->serialized_data != "" && deserialize(msg_cast->data, msg_cast->serialized_data);
}
template <> inline bool construct_msg<MessageBasePtr>(MessageBasePtr msg, MessageBasePtr msg_cast) {return false;}

class NetworkDevice {
 public:
    inline std::string get_topic() const {return topic_;}
    inline void set_topic(std::string topic) {topic_ = topic;}

    inline std::list<MessageBasePtr> &msg_list() {return msg_list_;}
    inline void set_msg_list(std::list<MessageBasePtr> msg_list) {msg_list_ = msg_list;}

    inline void set_max_queue_size(unsigned int size) { max_queue_size_ = size; }
    inline unsigned int max_queue_size() { return max_queue_size_; }

    template <class T=MessageBase> std::list<std::shared_ptr<T>> pop_msgs() {
        std::list<std::shared_ptr<T>> list_cast;
        while (!msg_list_.empty()) {
            auto msg = msg_list_.front();
            msg_list_.pop_front();
            std::shared_ptr<T> msg_cast = std::dynamic_pointer_cast<T>(msg);

            if (msg_cast) {                
                list_cast.push_back(msg_cast);
            } else if (msg->serialized_data != "") {                
                msg_cast = std::make_shared<T>();
//                msg_cast->serialized_data = msg->serialized_data;
//                if (construct_msg(msg, msg_cast)) {
//                    msg_cast->sender = msg->sender;
//#if ENABLE_PYTHON_BINDINGS==1
//                    msg_cast->py_data = msg->py_data; 
//#endif 
//                    list_cast.push_back(msg_cast);
//                }
            } else {
                std::cout << "Warning: failed to deliver message" << std::endl;
            }
        }
        return list_cast;
    }

    template <class T=MessageBase> std::list<std::shared_ptr<T>> msgs() {
        std::list<std::shared_ptr<T>> list_cast;
        for (auto &msg : msg_list_) {            
            std::shared_ptr<T> msg_cast = std::dynamic_pointer_cast<T>(msg);

            if (msg_cast) {                
                list_cast.push_back(msg_cast);
            } else if (msg->serialized_data != "") {                
                msg_cast = std::make_shared<T>();
//                msg_cast->serialized_data = msg->serialized_data;
//                if (construct_msg(msg, msg_cast)) {
//                    msg_cast->sender = msg->sender;
//#if ENABLE_PYTHON_BINDINGS==1
//                    msg_cast->py_data = msg->py_data; 
//#endif 
//                    list_cast.push_back(msg_cast);
//                }
            } else {
                std::cout << "Warning: failed to deliver message" << std::endl;
            }
        }
        return list_cast;
    }

    inline PluginPtr plugin() {return plugin_.lock();}

 protected:
    std::string topic_;
    std::list<MessageBasePtr> msg_list_;
    std::weak_ptr<Plugin> plugin_;
    unsigned int max_queue_size_;
};

}
#endif // Network_Device_H_
