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
#include <scrimmage/common/ID.h>
#include <scrimmage/entity/Contact.h>
#include <scrimmage/math/State.h>

namespace py = pybind11;
namespace sc = scrimmage;

void add_common(pybind11::module &m) {
    py::class_<sc::ID>(m, "ID")
        .def(py::init<int, int, int>())
        .def_property("id", &sc::ID::id, &sc::ID::set_id)
        .def_property("sub_swarm_id", &sc::ID::sub_swarm_id, &sc::ID::set_sub_swarm_id)
        .def_property("team_id", &sc::ID::team_id, &sc::ID::set_team_id);

    py::class_<sc::Contact, std::shared_ptr<sc::Contact>> contact(m, "Contact");

    contact.def(py::init<>())
           .def_property("id", &sc::Contact::id, &sc::Contact::set_id)
           .def_property("state", &sc::Contact::state, &sc::Contact::set_state)
           .def_property("type", &sc::Contact::type, &sc::Contact::set_type);

    py::enum_<sc::Contact::Type>(contact, "ContactType")
        .value("AIRCRAFT", sc::Contact::Type::AIRCRAFT)
        .value("QUADROTOR", sc::Contact::Type::QUADROTOR)
        .value("SPHERE", sc::Contact::Type::SPHERE)
        .value("MESH", sc::Contact::Type::MESH)
        .value("UNKNOWN", sc::Contact::Type::UNKNOWN)
        .export_values();

}

