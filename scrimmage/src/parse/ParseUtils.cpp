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

#include <scrimmage/parse/ParseUtils.h>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

namespace fs = boost::filesystem;
using std::cout;
using std::endl;

namespace scrimmage {

    bool find_terrain_files(std::string terrain_name,
                            ConfigParse &terrain_parse,
                            std::shared_ptr<scrimmage_proto::UTMTerrain> &utm_terrain)
    {
        if (utm_terrain == nullptr) {
            cout << "utm_terrain is null." << endl;
            return false;
        }

        utm_terrain->set_terrain_name(terrain_name);
            
        // Parse the motion model xml file
        terrain_parse.set_required("polydata");
        terrain_parse.set_required("texture");
        terrain_parse.set_required("system");
        terrain_parse.set_required("hemisphere");
        terrain_parse.set_required("zone");        
    
        FileSearch file_search;
        std::map<std::string, std::string> overrides; // empty, no overrides
        if(terrain_parse.parse(overrides, terrain_name, "SCRIMMAGE_DATA_PATH", file_search)) {        
            std::string polydata_file = terrain_parse.directory() + "/" + terrain_parse.params()["polydata"];
            std::string texture_file = terrain_parse.directory() + "/" + terrain_parse.params()["texture"];               
        
            if (fs::exists(polydata_file) && fs::exists(texture_file) &&
                fs::is_regular_file(polydata_file) && fs::is_regular_file(texture_file)) {
                utm_terrain->set_texture_file(texture_file);
                utm_terrain->set_poly_data_file(polydata_file);
                return true;
            }
        }
        return false;
    }

    bool find_model_properties(std::string model_name,
                               ConfigParse &cv_parse,
                               FileSearch &file_search, 
                               std::shared_ptr<scrimmage_proto::ContactVisual> &cv,
                               bool &mesh_found, bool &texture_found)
    {
        if (cv == nullptr) {
            cout << "Contact Visual is null." << endl;
            return false;
        }

        mesh_found = texture_found = false;
    
        cv->set_name(model_name);

        cv_parse.set_required("model");        
    
        std::map<std::string, std::string> overrides; // empty, no overrides
        if(cv_parse.parse(overrides, model_name, "SCRIMMAGE_DATA_PATH", file_search)) {        
            std::string model_file = cv_parse.directory() + "/" + cv_parse.params()["model"];
            std::string texture_file = cv_parse.directory() + "/" + cv_parse.params()["texture"];

            if (fs::exists(model_file) && fs::is_regular_file(model_file)) {
                cv->set_model_file(model_file);
                mesh_found = true;
            }
            if (fs::exists(texture_file) && fs::is_regular_file(texture_file)) {
                cv->set_texture_file(texture_file);
                texture_found = true;
            }
        }

        auto it_info = cv_parse.params().find("visual_scale");
        if (it_info != cv_parse.params().end()) {
            cv->set_scale(std::stod(it_info->second));
        } else {
            cv->set_scale(1.0);
        }
    
        bool valid_rotate = false;
        it_info = cv_parse.params().find("visual_rpy");
        if (it_info != cv_parse.params().end()) {
            std::vector<double> temp_rotate;
            if (str2vec<double>(it_info->second, " ", temp_rotate, 3)) {
                valid_rotate = true;
                for (auto it = temp_rotate.begin(); it != temp_rotate.end(); it++) {
                    cv->add_rotate(*it);
                }
            } else {
                cout << "Error parsing: visual_rpy" << endl;
            }            
        } 
        if (!valid_rotate) {
            for (int i = 0; i < 3; i++) {
                cv->add_rotate(0);
            }
        }
        return (mesh_found && texture_found);
    }

    bool parse_autonomy_data(std::map<std::string,std::string> &params,
                             std::map<std::string,std::string> &data_params)
    {        
        if (params.count("data") == 0) {
            cout << "Missing data tag in params" << endl;
            return false;
        }

        ConfigParse data;
        FileSearch file_search;
        std::map<std::string, std::string> overrides; // empty, no overrides
        if (data.parse(overrides, params["data"], "SCRIMMAGE_DATA_PATH",
                       file_search)) {
            data_params = data.params();
        } else {
            cout << "Failed to find data file: " << params["data"] << endl;
            return false;
        }
        return true;
    }

    std::string expand_user(std::string path)
    {
        if (not path.empty() and path[0] == '~') {
            assert(path.size() == 1 or path[1] == '/');  // or other error handling
            char const* home = std::getenv("HOME");
            if (home or ((home = std::getenv("USERPROFILE")))) {
                path.replace(0, 1, home);
            } else {
                char const *hdrive = std::getenv("HOMEDRIVE"),
                    *hpath = std::getenv("HOMEPATH");
                assert(hdrive);  // or other error handling
                assert(hpath);
                path.replace(0, 1, std::string(hdrive) + hpath);
            }
        }
        return path;
    }

    bool str2bool(std::string str)
    {
        // Remove spaces
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace),
                  str.end());

        if (boost::to_upper_copy(str) == "TRUE") {
            return true;
        }

        // Maybe it's a "1" or "0"
        int num;
        if ( ! (std::istringstream(str) >> num) ) num = 0;
        return (num == 1);
    }    

    bool get_vec(std::string str, 
                 std::map<std::string,std::string> & params,
                 std::vector<std::string> &vec)
    {
        vec.clear();
    
        // Determine if the string name is a list
        int count = get(str+":size", params, 0);
        if (count <= 0) return false;
    
        for (int i = 0; i < count; i++) {
            std::string name = str;
            if (i > 0) {
                name += "_" + std::to_string(i);
            } 
            if (params.count(name) == 0) {
                cout << "Invalid name in params: " << str << endl;
                return false;
            }
            vec.push_back(params[name]);
        }
        return true;
    }

    Eigen::Vector3d vec2eigen(std::vector<double> &vec) {        
        Eigen::Vector3d v(0,0,0);        
        if (vec.size() <= 3) {
            for (unsigned int i = 0; i < vec.size(); i++) {
                v(i) = vec[i];
            }
        }
        return v;
    }

}
