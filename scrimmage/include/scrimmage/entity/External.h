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
#ifndef EXTERNAL_H_
#define EXTERNAL_H_

#include <scrimmage/fwd_decl.h>
#include <scrimmage/common/ID.h>
#include <scrimmage/common/FileSearch.h>
#include <map>

namespace scrimmage {

class External {
 public:
    External();
    NetworkPtr &network();
    EntityPtr &entity();
    FileSearch &file_search();
    PluginManagerPtr &plugin_manager();
    int get_max_entities();
    void set_max_entities(int max_entities);
    bool create_entity(ID id, std::map<std::string, std::string> &info);

 protected: 
    NetworkPtr network_;
    EntityPtr entity_;
    FileSearch file_search_; 
    PluginManagerPtr plugin_manager_;
    int max_entities_;
};

} // namespace scrimmage

#endif // EXTERNAL_H_
