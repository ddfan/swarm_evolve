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
#include <pybind11/pybind11.h>
#include <scrimmage/autonomy/Autonomy.h>
#include <scrimmage/entity/Contact.h>

class PyAutonomy : public scrimmage::Autonomy {
 public:
    PyAutonomy();
    virtual void init(std::map<std::string,std::string> &params);
    virtual bool step_autonomy(double t, double dt);

    std::string type() {return std::string("PyAutonomy");}

    virtual void set_contacts(scrimmage::ContactMapPtr &contacts) {
        contacts_ = contacts;
        py_contacts_.clear();
        for (auto &kv : *contacts) {
            py_contacts_[pybind11::int_(kv.second.id().id())] =
                contact2py(kv.second);
        }
    }

    virtual void set_contacts_from_plugin(scrimmage::AutonomyPtr &ptr) {
        std::shared_ptr<PyAutonomy> py_ptr =
            std::static_pointer_cast<PyAutonomy>(ptr);
        contacts_ = py_ptr->contacts_;
        py_contacts_ = py_ptr->py_contacts_;
    }

    virtual void set_state(scrimmage::StatePtr &state) {
        state_ = state;
        py_state_ = state2py(state);
    }

 protected:

    void cache_python_vars();
    pybind11::object get_py_obj(std::map<std::string, std::string> &params);
    void init_py_obj(std::map<std::string, std::string> &params);

    void sub_msgs_to_py_subs();
    void py_pub_msgs_to_pubs();
    void sync_topics();

    void print_py_traceback(pybind11::error_already_set &e);

    bool step_autonomy_called_ = false;

    pybind11::object state2py(scrimmage::StatePtr &state);
    pybind11::object contact2py(scrimmage::Contact contact);
    static std::shared_ptr<scrimmage_proto::Shape> py2shape(const pybind11::handle& py_handle);

    pybind11::object py_obj_;
    pybind11::dict py_contacts_;

    // cached imports
    pybind11::object py_state_class_;
    pybind11::object py_quat_class_;
    pybind11::object py_contact_class_;
    pybind11::object py_msg_class_;
    pybind11::object py_state_;
    pybind11::object py_id_class_;

    bool serialize_msgs_ = false;

    std::map<scrimmage::Contact::Type, pybind11::object> py_contact_types_;
};
