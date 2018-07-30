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
#ifndef _SCRIMMAGE_MESSAGE_BASE_H_
#define _SCRIMMAGE_MESSAGE_BASE_H_

#include <string>
#include <memory>

#if ENABLE_PYTHON_BINDINGS==1
#include <pybind11/pybind11.h>
#endif

namespace scrimmage {

class MessageBase {
 public:
    virtual ~MessageBase() {}       //http://stackoverflow.com/a/5831797

    static const int undefined_id = -1;
    int sender;
    double time;
    std::string serialized_data;

#if ENABLE_PYTHON_BINDINGS==0
    MessageBase(int _sender=undefined_id, std::string _serialized_data="");
#else 
    MessageBase(int _sender=undefined_id, std::string _serialized_data="", pybind11::object _py_data=pybind11::none());

    void serialize_to_python(std::string module_name, std::string object_name);

    pybind11::object py_data;
#endif 
};

using MessageBasePtr = std::shared_ptr<MessageBase>;

} // namespace scrimmage
#endif
