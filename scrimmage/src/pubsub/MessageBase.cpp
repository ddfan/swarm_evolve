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
#include <scrimmage/pubsub/MessageBase.h>

#if ENABLE_PYTHON_BINDINGS==1
namespace py = pybind11;
#endif

namespace scrimmage {

#if ENABLE_PYTHON_BINDINGS==0
    MessageBase::MessageBase(int _sender, std::string _serialized_data) :
        sender(_sender), serialized_data(_serialized_data) {}
#else
MessageBase::MessageBase(int _sender, std::string _serialized_data, pybind11::object _py_data) :
    sender(_sender), serialized_data(_serialized_data), py_data(_py_data) {}

void MessageBase::serialize_to_python(std::string module_name, std::string object_name) {
#if ENABLE_PYTHON_BINDINGS==0
    return;
#else 
    if (serialized_data == "") {
        return;
    }
    py::module pb_module = pybind11::module::import(module_name.c_str());
    py::object pb_object_class = pb_module.attr(object_name.c_str());
    py_data = pb_object_class();
    py::object parse_func = py_data.attr("ParseFromString");
    parse_func(pybind11::bytes(serialized_data));
#endif 
}

#endif

}
