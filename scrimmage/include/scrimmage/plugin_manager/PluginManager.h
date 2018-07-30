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
#ifndef PLUGINMANAGER_H_
#define PLUGINMANAGER_H_

#include <list>
#include <map>
#include <unordered_set>
#include <memory>

#include <scrimmage/fwd_decl.h>
#include <scrimmage/common/FileSearch.h>
#include <scrimmage/parse/ConfigParse.h>

namespace scrimmage {


class PluginInfo {
 public:
    PluginInfo() { }
    std::string name;
    std::string type;
    std::string path;
    bool returned = false;
    void * handle;
};

class PluginManager {
 public:

    typedef const char * (*plugin_name_t)();
    typedef const char * (*plugin_type_t)();

    PluginManager();

    void print_plugins(std::string plugin_type, std::string title, FileSearch &file_search);
    void print_returned_plugins();
    PluginPtr make_plugin(std::string plugin_type, std::string &plugin_name_xml, FileSearch &file_search, ConfigParse &config_parse, std::map<std::string, std::string> &overrides);
    std::map<std::string, std::unordered_set<std::string>> get_commits();
    void set_reload(bool reload);
    bool get_reload();

 protected:
    // Key 1: Plugin Type
    // Key 2: Plugin Name
    // Value: PluginInfo
    std::map<std::string, std::map<std::string, PluginInfo>> plugins_;

    std::unordered_map<std::string, std::list<std::string>> so_files_;
    bool files_checked_ = false;

    int check_library(std::string lib_path);
    PluginPtr make_plugin_helper(std::string &plugin_type, std::string &plugin_name);
    bool reload_;
};

using PluginManagerPtr = std::shared_ptr<PluginManager>;
}

#endif
