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
#ifndef _SCRIMMAGE_MESSAGE_H_
#define _SCRIMMAGE_MESSAGE_H_

#include <scrimmage/pubsub/MessageBase.h>

namespace scrimmage {

class MessageBase; 

template <class T>
class Message : public MessageBase {
 public:
    Message() : MessageBase() {}
#if ENABLE_PYTHON_BINDINGS==0
    Message(T _data, int _sender=undefined_id, std::string _serialized_data="") :
        MessageBase(_sender, _serialized_data), data(_data) {}
#else 
    Message(T _data, int _sender=undefined_id, std::string _serialized_data="", pybind11::object _py_data=pybind11::none()) :
        MessageBase(_sender, _serialized_data, _py_data), data(_data) {}
#endif 
    T data;

};

} // namespace scrimmage
#endif
