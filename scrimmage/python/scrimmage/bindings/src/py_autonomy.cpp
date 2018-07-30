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
#include <py_utils.h>
#include <scrimmage/autonomy/Autonomy.h>
#include <scrimmage/plugin_manager/Plugin.h>
#include <scrimmage/pubsub/Network.h>
#include <scrimmage/math/State.h>
#include <scrimmage/pubsub/Publisher.h>
#include <scrimmage/pubsub/Subscriber.h>

namespace py = pybind11;
namespace sc = scrimmage;

void add_autonomy(pybind11::module &m) {
    py::class_<sc::Plugin, std::shared_ptr<sc::Plugin>> plugin(m, "Plugin");
    plugin.def(py::init<>())
        .def("name", &sc::Plugin::name)
        .def("type", &sc::Plugin::type)        
        .def_property("network", &sc::Plugin::network, &sc::Plugin::set_network)
        .def_property("pubs", &sc::Plugin::pubs, &sc::Plugin::set_pubs)
        .def_property("subs", &sc::Plugin::subs, &sc::Plugin::set_subs)
        .def("create_publisher", &sc::Plugin::create_publisher)
        .def("create_subscriber", &sc::Plugin::create_subscriber)
        .def("stop_publishing", &sc::Plugin::stop_publishing)
        .def("stop_subscribing", &sc::Plugin::stop_subscribing);

    py::class_<sc::Autonomy, std::shared_ptr<sc::Autonomy>>(m, "Autonomy", plugin)
        .def(py::init<>())
        .def_property("desired_state", &sc::Autonomy::desired_state, &sc::Autonomy::set_desired_state)                
        .def("name", &sc::Autonomy::name)
        .def("type", &sc::Autonomy::type)        
        .def("step_autonomy", &sc::Autonomy::step_autonomy)
        .def("get_contacts", &sc::Autonomy::get_contacts_raw)
        .def("shapes", &sc::Autonomy::shapes); // TODO, anyone using shapes in Python yet?
}
