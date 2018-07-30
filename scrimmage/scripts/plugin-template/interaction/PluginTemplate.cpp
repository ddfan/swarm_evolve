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
/// @author Laura Strickland <laura.strickland@gtri.gatech.edu> 
/// @version 1.0
/// ---------------------------------------------------------------------------
/// @brief A brief description.
/// 
/// @section DESCRIPTION
/// A long description.
/// ---------------------------------------------------------------------------
#include <memory>
#include <limits>
#include <iostream>

#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/common/Entity.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/math/State.h>

#include "(>>>PLUGIN_NAME<<<).h"

using std::cout;
using std::endl;

namespace sc = scrimmage;

REGISTER_PLUGIN(scrimmage::EntityInteraction, (>>>PLUGIN_NAME<<<), (>>>PLUGIN_NAME<<<)_plugin)

(>>>PLUGIN_NAME<<<)::(>>>PLUGIN_NAME<<<)()
{     
}

bool (>>>PLUGIN_NAME<<<)::init(std::map<std::string,std::string> &mission_params,
                           std::map<std::string,std::string> &plugin_params)
{
    return true;
}


bool (>>>PLUGIN_NAME<<<)::step_entity_interaction(std::list<sc::EntityPtr> &ents, 
                                                  double t, double dt)
{
    if (ents.empty())
    {
        return true;
    }

    return true;
}
