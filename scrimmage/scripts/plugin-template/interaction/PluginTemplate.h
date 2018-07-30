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
#ifndef (>>>PLUGIN_NAME<<<)_H_
#define (>>>PLUGIN_NAME<<<)_H_
#include <scrimmage/simcontrol/EntityInteraction.h>
#include <scrimmage/common/Entity.h>

namespace sc = scrimmage;

class (>>>PLUGIN_NAME<<<) : public scrimmage::EntityInteraction
{
    public:
        (>>>PLUGIN_NAME<<<)();
        bool init(std::map<std::string,std::string> &mission_params,
                std::map<std::string,std::string> &plugin_params);
        bool step_entity_interaction(std::list<sc::EntityPtr> &ents,
                                     double t, double dt);
    protected:
    private:     
};

#endif
