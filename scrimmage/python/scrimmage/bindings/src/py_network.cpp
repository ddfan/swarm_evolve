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

#include <pybind11/pybind11.h>
#include <scrimmage/pubsub/Publisher.h>
#include <scrimmage/pubsub/Subscriber.h>
#include <scrimmage/pubsub/MessageBase.h>
#include <scrimmage/pubsub/Network.h>
#include <py_utils.h>

namespace py = pybind11;
namespace sc = scrimmage;

void add_network(pybind11::module &m) {
    py::class_<sc::MessageBase, std::shared_ptr<sc::MessageBase>>(m, "Message")
        .def(py::init<>())
        .def(py::init<int>())
        .def(py::init<int, std::string>())
        .def(py::init<int, std::string, py::object>())
        .def_readwrite("sender", &sc::MessageBase::sender)
        .def_readwrite("serialized_data", &sc::MessageBase::serialized_data)
        .def_readwrite("data", &sc::MessageBase::py_data);

    py::class_<sc::NetworkDevice, std::shared_ptr<sc::NetworkDevice>>(m, "NetworkDevice")
        .def(py::init<>())
        .def_property("topic", &sc::NetworkDevice::get_topic, &sc::NetworkDevice::set_topic)
        .def_property("msg_list", &sc::NetworkDevice::msg_list, &sc::NetworkDevice::set_msg_list);

    py::class_<sc::Publisher, std::shared_ptr<sc::Publisher>, sc::NetworkDevice>(m, "Publisher")
        .def(py::init<>())
        .def("publish", &sc::Publisher::publish);

    py::class_<sc::Subscriber, std::shared_ptr<sc::Subscriber>, sc::NetworkDevice>(m, "Subscriber")
        .def(py::init<>());

    py::class_<sc::Network, std::shared_ptr<sc::Network>>(m, "Network")
        .def(py::init<>())
        .def("add_publisher", &sc::Network::add_publisher)
        .def("add_subscriber", &sc::Network::add_subscriber)
        .def("rm_publisher", &sc::Network::rm_publisher)
        .def("rm_publisher", &sc::Network::rm_publisher);
}
