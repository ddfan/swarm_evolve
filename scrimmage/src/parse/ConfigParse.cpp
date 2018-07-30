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
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <scrimmage/parse/ConfigParse.h>
#include <scrimmage/common/Utilities.h>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

#include "rapidxml/rapidxml.hpp"

namespace fs = boost::filesystem;
using namespace rapidxml;
using std::cout;
using std::endl;

namespace scrimmage {

    ConfigParse::ConfigParse()
    {

    }

    void ConfigParse::set_required(std::string node_name)
    {
        required_.push_back(node_name);
    }

    void recursive_params(xml_node<> *root,
                          std::map<std::string, std::string> &overrides,
                          std::map<std::string, std::string> &params,
                          std::string prev)
    {
        // End condition
        if (root == 0 || root->name() == std::string("")
            || root->name() == std::string(" ")) {
            return;
        }

        // Process this node
        std::string name = root->name();
        if (prev != "") {
            name = prev + "/" + name;
        }

        if (params.count(name) > 0) {
            // Name already exists, consider this a list. Does the size
            //parameter exist yet?
            std::string name_size = name + ":size";
            int size = 2;
            if (params.count(name_size) > 0) {
                // It does exist, grab the current size and increment.
                size = std::stoi(params[name_size]) + 1;                
            }   
            name = name + "_" + std::to_string(size-1);
            params[name_size] = std::to_string(size);
        }

        if (overrides.count(name)) {
            params[name] = overrides[name];
        } else {
            params[name] = root->value();
        }

        // Recurse (depth-first)
        recursive_params(root->first_node(), overrides, params, name);

        // Recurse (sibling node)
        recursive_params(root->next_sibling(), overrides, params, prev);
    }

    bool ConfigParse::parse(std::map<std::string, std::string> &overrides,
                            std::string filename, std::string env_var,
                            FileSearch &file_search)
    {
        bool status = file_search.find_file(filename, "xml", env_var, filename);
        if (!status) {
            // cout << "Failed to find config: " << filename << endl;
            return false;
        }

        filename_ = filename;

        xml_document<> doc;
        std::ifstream file(filename_.c_str());
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        std::string content(buffer.str());
        doc.parse<0>(&content[0]);

        xml_node<> *config_node = doc.first_node("params");
        if (config_node == 0) {
            cout << "Missing tag: params" << endl;
            return false;
        }

        params_.clear();
        params_["XML_DIR"] = this->directory() + "/";
        recursive_params(config_node->first_node(), overrides, params_, "");

        for (std::string &node_name : required_) {
            if (params_.count(node_name) == 0) {
                cout << "Config file is missing XML tag: " << node_name << endl;
                return false;
            }
        }
        return true;
    }

    std::map<std::string, std::string> &ConfigParse::params() { return params_; }

    std::string ConfigParse::filename() { return filename_; }

    std::string ConfigParse::directory()
    {
        if (fs::exists(filename_)) {
            return fs::path(filename_).parent_path().string();
        }
        return "";
    }
    std::string ConfigParse::extension()
    {
        if (fs::exists(filename_)) {
            return fs::path(filename_).extension().string();
        }
        return "";
    }

    std::string ConfigParse::stem()
    {
        if (fs::exists(filename_)) {
            return fs::path(filename_).stem().string();
        }
        return "";
    }

    void ConfigParse::print_params()
    {
        for (std::map<std::string, std::string>::iterator it = params_.begin();
             it != params_.end(); it++) {
            cout << it->first << "=" << it->second << endl;
        }
    }
}
