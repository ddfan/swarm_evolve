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
#include <Eigen/Dense>
#include <scrimmage/math/Quaternion.h>
#include <scrimmage/entity/Contact.h>
#include <scrimmage/log/Log.h>
#include <py_utils.h>
#include <string>

namespace py = pybind11;
namespace sc = scrimmage;

class MyException : public std::exception {
public:
    explicit MyException(const char * m) : message{m} {}
    virtual const char * what() const noexcept override {return message.c_str();}
private:
    std::string message = "";
};

pybind11::object frames2pandas(std::string &fname) {

    // TODO: not sure if this is working after log refactor
    scrimmage::Log log;
    if (!log.parse(fname, scrimmage::Log::FRAMES)) {
        throw MyException((fname + "does not exist").c_str());
    }

    pybind11::list data;
    for (scrimmage::Frame &frame : log.scrimmage_frames()) {
        for (auto &kv : *(frame.contacts_)) {
            unsigned int id = kv.first;
            scrimmage::Contact &c = kv.second;

            Eigen::Vector3d &p = c.state()->pos();
            Eigen::Vector3d &v = c.state()->vel();
            scrimmage::Quaternion &q = c.state()->quat();

            pybind11::list row;
            row.append(pybind11::float_(frame.time_));
            row.append(pybind11::int_(id));
            row.append(pybind11::int_(c.id().team_id()));
            row.append(pybind11::int_(c.id().sub_swarm_id()));
            int type = static_cast<int>(c.type());
            row.append(pybind11::int_(type));
            row.append(pybind11::float_(p(0)));
            row.append(pybind11::float_(p(1)));
            row.append(pybind11::float_(p(2)));
            row.append(pybind11::float_(v(0)));
            row.append(pybind11::float_(v(1)));
            row.append(pybind11::float_(v(2)));
            row.append(pybind11::float_(q.yaw()));
            row.append(pybind11::float_(q.pitch()));
            row.append(pybind11::float_(q.roll()));

            data.append(row);
        }
    }

    pybind11::list cols;
    cols.append(pybind11::str("time"));
    cols.append(pybind11::str("id"));
    cols.append(pybind11::str("team_id"));
    cols.append(pybind11::str("swarm_id"));
    cols.append(pybind11::str("veh_type"));
    cols.append(pybind11::str("x"));
    cols.append(pybind11::str("y"));
    cols.append(pybind11::str("z"));
    cols.append(pybind11::str("vx"));
    cols.append(pybind11::str("vy"));
    cols.append(pybind11::str("vz"));
    cols.append(pybind11::str("yaw"));
    cols.append(pybind11::str("pitch"));
    cols.append(pybind11::str("roll"));

    pybind11::object df_class_obj = 
        pybind11::module::import("pandas").attr("DataFrame");
    pybind11::object df = df_class_obj(data, pybind11::none(), cols);
    return df;
}
