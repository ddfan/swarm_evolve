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
#ifndef AUTONOMY_H_
#define AUTONOMY_H_

#include <list>
#include <map>
#include <memory>
#include <unordered_map>

#include <scrimmage/fwd_decl.h>
#include <scrimmage/plugin_manager/Plugin.h>
#include <scrimmage/entity/Contact.h>

namespace scrimmage_proto {
    class Shape;
    using ShapePtr = std::shared_ptr<Shape>;
}

namespace scrimmage {

class Autonomy : public Plugin {
 public:
    Autonomy();

    std::string type();
    virtual bool step_autonomy(double t, double dt);
    virtual bool posthumous(double t);
    virtual void init();
    virtual void init(std::map<std::string,std::string> &params);
    bool need_reset();

    // getters/setters
    StatePtr &desired_state();
    void set_desired_state(StatePtr desired_state);

    ContactMapPtr &get_contacts();
    ContactMap &get_contacts_raw();
    virtual void set_contacts(ContactMapPtr &contacts);
    virtual void set_contacts_from_plugin(AutonomyPtr &ptr);

    scrimmage::RTreePtr &rtree();
    void set_rtree(scrimmage::RTreePtr &rtree);

    StatePtr &state();
    virtual void set_state(StatePtr &state);

    std::list<scrimmage_proto::ShapePtr> &shapes();

    virtual void set_projection(std::shared_ptr<GeographicLib::LocalCartesian> proj);

    std::string &logging_msg();

    bool get_is_controlling();
    void set_is_controlling(bool is_controlling);

 protected:
    std::shared_ptr<GeographicLib::LocalCartesian> proj_;

    StatePtr state_;
    StatePtr desired_state_;
    ContactMapPtr contacts_;    
    scrimmage::RTreePtr rtree_;

    std::list<scrimmage_proto::ShapePtr> shapes_;
    bool need_reset_;
    std::string logging_msg_;

    bool is_controlling_;
};
}
#endif
