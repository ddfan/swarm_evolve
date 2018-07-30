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
#ifndef CONTACT_H_
#define CONTACT_H_
#include <scrimmage/fwd_decl.h>
#include <scrimmage/common/ID.h>
#include <memory>
#include <unordered_map>
#include <list>

#include <scrimmage/proto/Visual.pb.h>

namespace scrimmage {

using ContactVisualPtr = std::shared_ptr<scrimmage_proto::ContactVisual>;

class Contact {
 public:

    enum class Type {AIRCRAFT = 0, QUADROTOR, SPHERE, MESH, UNKNOWN};

    Contact();

    Contact(ID &id, StatePtr &state, Type type,
            ContactVisualPtr cv,
            std::unordered_map<std::string, std::list<SensablePtr>> sp);
    
    void set_id(const ID &id);
    ID &id();

    void set_state(StatePtr &state);
    StatePtr &state();

    void set_type(Type type);
    Type type();

    ContactVisualPtr & contact_visual();

    std::unordered_map<std::string, std::list<SensablePtr>> &sensables();

    void set_active(bool active);
    bool active();

 protected:
    ID id_;
    StatePtr state_;
    Type type_;
    ContactVisualPtr contact_visual_;
    std::unordered_map<std::string, std::list<SensablePtr>> sensables_;
    bool active_;
};

using ContactMap = std::unordered_map<int, Contact>;
using ContactMapPtr = std::shared_ptr<ContactMap>;
}
#endif
