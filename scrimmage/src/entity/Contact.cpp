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

#include <scrimmage/entity/Contact.h>
#include <scrimmage/math/State.h>
#include <scrimmage/common/ID.h>

namespace scrimmage {

Contact::Contact() : state_(std::make_shared<State>()) {}

Contact::Contact(ID &id, StatePtr &state, Type type,
                 ContactVisualPtr cv,
                 std::unordered_map<std::string, std::list<SensablePtr>> sp) :
    id_(id), state_(state), type_(type), contact_visual_(cv),
    sensables_(sp), active_(true) {}

void Contact::set_id(const ID &id) { id_ = id; }

ID &Contact::id() { return id_; }

void Contact::set_state(StatePtr &state) { state_ = state; }

StatePtr &Contact::state() { return state_; }

void Contact::set_type(Contact::Type type) { type_ = type; }

Contact::Type Contact::type() { return type_; }

ContactVisualPtr &Contact::contact_visual()
{ return contact_visual_; }

std::unordered_map<std::string, std::list<SensablePtr> > &Contact::sensables() {
    return sensables_;
}

void Contact::set_active(bool active) { active_ = active; }

bool Contact::active() { return active_; }

} // namespace scrimmage
