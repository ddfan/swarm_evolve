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
#include <scrimmage/plugin_manager/PluginManager.h>
#include <scrimmage/plugin_manager/Plugin.h>
#include <scrimmage/common/Utilities.h>
#include <iostream>
#include <dlfcn.h>
#include <scrimmage/parse/ConfigParse.h>
#include <boost/filesystem.hpp>

namespace scrimmage {

int PluginManager::check_library(std::string lib_path)
{                                    
    void *lib_handle;
    lib_handle = dlopen(lib_path.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if(!lib_handle){
        fputs (dlerror(), stderr);
        std::cout << std::endl;
        return -1;
    }

    char * error;

    // Extract the plugin type
    plugin_type_t type_func;
    type_func = (plugin_type_t)dlsym(lib_handle, "plugin_type");
    if ((error = dlerror()) != NULL)  {
        //fputs(error, stderr);
        std::cout << lib_path << " doesn't contain 'plugin_type'" << std::endl;
        dlclose(lib_handle);
        return 1;
    }
    std::string plugin_type((*type_func)());

    // Extract the plugin name
    plugin_name_t name_func;
    name_func = (plugin_name_t)dlsym(lib_handle, "plugin_name");
    if ((error = dlerror()) != NULL)  {
        //fputs(error, stderr);
        std::cout << lib_path << " doesn't contain 'plugin_name'" << std::endl;
        dlclose(lib_handle);
        return 3;
    }
    std::string plugin_name((*name_func)());

    // Ensure the maker function exists, but we don't need to call it
    dlsym(lib_handle, "maker");
    if ((error = dlerror()) != NULL)  {
        //fputs(error, stderr);
        std::cout << lib_path << " doesn't contain 'maker'" << std::endl;
        dlclose(lib_handle);
        return 4;
    }

    PluginInfo info;
    info.name = plugin_name;
    info.type = plugin_type;
    info.path = lib_path;
    info.handle = lib_handle;

    plugins_[plugin_type][plugin_name] = info;
    
    //dlclose(lib_handle);

    return 0;
}

PluginManager::PluginManager() : reload_(false) {}

void PluginManager::print_plugins(std::string plugin_type, std::string title, FileSearch &file_search)
{
    // make sure all files are loaded
    if (!files_checked_) {
        file_search.find_files("SCRIMMAGE_PLUGIN_PATH", ".so", so_files_);
        files_checked_ = true;
    }

    for (auto &kv : so_files_) {
        for (std::string &full_path : kv.second) {
            check_library(full_path);
        }
    }

    std::cout << "------------------------------" << std::endl;
    std::cout << title << ": " << std::endl;
    std::cout << "------------------------------" << std::endl;
    if (plugins_.count(plugin_type) > 0) {
        for (auto &kv : plugins_[plugin_type]) {
            std::cout << kv.first << std::endl;
        }
    } else {
        std::cout << "Plugin type doesn't exist: " << plugin_type << std::endl;
    }
    std::cout << "------------------------------" << std::endl;
}

void PluginManager::print_returned_plugins() {
    std::cout << "using the following plugins:" << std::endl;
    for (auto &kv : plugins_) {
        for (auto &kv2 : kv.second) {
            if (kv2.second.returned) {
                std::cout << kv.first << "::" << kv2.first << std::endl;
            }
        }
    }
}

PluginPtr PluginManager::make_plugin_helper(std::string &plugin_type, std::string &plugin_name) {
    auto it = plugins_.find(plugin_type);
    if (it != plugins_.end()) {
        auto it2 = it->second.find(plugin_name);
        if (it2 != it->second.end()) {
            it2->second.returned = true;

            if (reload_ && it2->second.handle) {
                dlclose(it2->second.handle);
            }

            PluginPtr (*maker_func)(void);
            maker_func = (PluginPtr (*)(void))dlsym(it2->second.handle, "maker");
            char * error;
            if ((error = dlerror()) != NULL)  {
                fputs(error, stderr);
                return nullptr;
            } else {
                return (*maker_func)();
            }
        }
    }
    return nullptr;
}

PluginPtr PluginManager::make_plugin(std::string plugin_type,
                                     std::string &plugin_name_xml,
                                     FileSearch &file_search,
                                     ConfigParse &config_parse,
                                     std::map<std::string, std::string> &overrides)
{
    config_parse.set_required("library");
    if (!config_parse.parse(overrides, plugin_name_xml, "SCRIMMAGE_PLUGIN_PATH", file_search)) {
        std::cout << "Failed to parse: " << plugin_name_xml << ".xml for type " << plugin_type << std::endl;
        std::cout << "Check that you have sourced setenv.sh (this sets the environment variable SCRIMMAGE_PLUGIN_PATH to a directory including " << plugin_name_xml << ".xml)" << std::endl;
        return nullptr;
    }

    std::string plugin_name_so = config_parse.params()["library"];

    // first, if this has already been processed, return it
    PluginPtr plugin = make_plugin_helper(plugin_type, plugin_name_so);
    if (plugin != nullptr) {
        return plugin;
    }

    if (!files_checked_ && so_files_.empty()) {
        file_search.find_files("SCRIMMAGE_PLUGIN_PATH", ".so", so_files_);
        files_checked_ = true;
    }

    // try the most obvious case, that the lib is named the same as the
    // plugin_name
    auto it = so_files_.find(std::string("lib") + plugin_name_so + ".so");
    if (it != so_files_.end()) {
        for (std::string &full_fname : it->second) {
            if (check_library(full_fname) == 0) {
                // don't need to check again that it is in the map since the helper
                // will do this already
                so_files_.erase(it);
                PluginPtr ptr = make_plugin_helper(plugin_type, plugin_name_so);
                return ptr;
            }
        }
    }

    // last, loop through the files
    it = so_files_.begin();
    while (it != so_files_.end()) {
        for (std::string &full_fname : it->second) {
            if (check_library(full_fname) == 0) {
                // don't need to check again that it is in the map since the helper
                // will do this already
                so_files_.erase(it);
                return make_plugin_helper(plugin_type, plugin_name_so);
            }
        }
        it++;
    }

    std::cout << "could not locate " << plugin_type << "::" << plugin_name_so << std::endl;
    return nullptr;
}

std::map<std::string, std::unordered_set<std::string>> PluginManager::get_commits() {
    std::map<std::string, std::unordered_set<std::string>> commits;
    std::string sha;
    for (auto &kv : plugins_) {
        for (auto &kv2 : kv.second) {
            scrimmage::PluginInfo &plugin_info = kv2.second;
            if (!plugin_info.returned) continue;

            std::string path =
                boost::filesystem::path(plugin_info.path).parent_path().string();

            if ((sha = scrimmage::get_sha(path)) != "") {
                commits[sha].insert(plugin_info.name);
            }
        }
    }
    return commits;
}

void PluginManager::set_reload(bool reload) {reload_ = reload;}

bool PluginManager::get_reload() {return reload_;}

} // namespace scrimmage
