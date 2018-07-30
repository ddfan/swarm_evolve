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
#include <pybind11/eigen.h>
#include <scrimmage/math/Quaternion.h>
#include <scrimmage/math/State.h>
#include <py_utils.h>

namespace py = pybind11;
namespace sc = scrimmage;

void add_math(pybind11::module &m) {
    py::class_<sc::Quaternion>(m, "Quaternion")
        .def(py::init<double, double, double, double>(),
             py::arg("w"), py::arg("x"), py::arg("y"), py::arg("z"))
        .def(py::init<double, double, double>(),
             py::arg("roll"), py::arg("pitch"), py::arg("yaw"))
        .def(py::init<Eigen::Vector3d, double>(),
             py::arg("vector"), py::arg("scalar"))
        .def_property_readonly("w", (double &(sc::Quaternion::*)()) &sc::Quaternion::w)
        .def_property_readonly("x", (double &(sc::Quaternion::*)()) &sc::Quaternion::x)
        .def_property_readonly("y", (double &(sc::Quaternion::*)()) &sc::Quaternion::y)
        .def_property_readonly("z", (double &(sc::Quaternion::*)()) &sc::Quaternion::z)
        .def("set", (void (sc::Quaternion::*)(double, double, double)) &sc::Quaternion::set)
        .def("set", (void (sc::Quaternion::*)
                    (const Eigen::Vector3d &vector, double angle))
                    &sc::Quaternion::set)
        .def("roll", &sc::Quaternion::roll)
        .def("yaw", &sc::Quaternion::yaw)
        .def("pitch", &sc::Quaternion::pitch)
        .def("yaw", &sc::Quaternion::yaw)
        .def("rotation_angle", &sc::Quaternion::rotation_angle)
        .def("rotate", &sc::Quaternion::rotate)
        .def("rotate_reverse", &sc::Quaternion::rotate_reverse);

    py::class_<sc::State, std::shared_ptr<sc::State>>(m, "State")
        .def(py::init<>())
        .def(py::init<Eigen::Vector3d, Eigen::Vector3d, sc::Quaternion>(),
             py::arg("position"), py::arg("velocity"), py::arg("orientation"))
        .def_property("pos", &sc::State::pos, &sc::State::set_pos)
        .def_property("vel", &sc::State::vel, &sc::State::set_vel)
        .def_property("quat", &sc::State::quat, &sc::State::set_quat)
        .def("in_field_of_view", &sc::State::InFieldOfView)
        .def("rel_pos_local_frame", &sc::State::rel_pos_local_frame)
        .def("orient_global_frame", &sc::State::orient_global_frame);
}
