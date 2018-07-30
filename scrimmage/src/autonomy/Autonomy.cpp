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
#include <memory>

#include <scrimmage/autonomy/Autonomy.h>
//#include <scrimmage/proto/Shape.pb.h>
#include <scrimmage/math/State.h>
#include <GeographicLib/LocalCartesian.hpp>

namespace scrimmage {

Autonomy::Autonomy() : state_(std::make_shared<State>()),
    desired_state_(std::make_shared<State>()), need_reset_(false)
{
}


void Autonomy::set_contacts(ContactMapPtr &contacts)
{
    contacts_ = contacts;
}

void Autonomy::set_contacts_from_plugin(AutonomyPtr &ptr)
{
    contacts_ = ptr->contacts_;
}

RTreePtr &Autonomy::rtree() {return rtree_;}

void Autonomy::set_rtree(RTreePtr &rtree) {rtree_ = rtree;}

StatePtr &Autonomy::state() {return state_;}

void Autonomy::set_state(StatePtr &state) {state_ = state;}

std::list<scrimmage_proto::ShapePtr> &Autonomy::shapes() {return shapes_;}

void Autonomy::set_projection(std::shared_ptr<GeographicLib::LocalCartesian> proj) {
    proj_ = proj;
}

std::string &Autonomy::logging_msg() {return logging_msg_;}

bool Autonomy::get_is_controlling() {return is_controlling_;}

void Autonomy::set_is_controlling(bool is_controlling) {is_controlling_ = is_controlling;}

std::string Autonomy::type() { return std::string("Autonomy"); }

bool Autonomy::step_autonomy(double t, double dt) { return false; }
bool Autonomy::posthumous(double t) { return true; }
void Autonomy::init() {}
void Autonomy::init(std::map<std::string, std::string> &params) {}
bool Autonomy::need_reset() {return need_reset_;}

StatePtr &Autonomy::desired_state() {return desired_state_;}

void Autonomy::set_desired_state(StatePtr desired_state) {desired_state_ = desired_state;}

ContactMapPtr &Autonomy::get_contacts() {return contacts_;}

ContactMap &Autonomy::get_contacts_raw() {return *contacts_;}

} // namespace scrimmage
