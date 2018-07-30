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
#include <sstream>
#include <string>
#include <cassert>
#include <vector>
#include <memory>
#include <scrimmage/common/Utilities.h>
#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
namespace fs = boost::filesystem;

using std::cout;
using std::endl;

namespace scrimmage
{
int next_available_id(std::string name,
                      std::map<std::string, std::string> &info,
                      std::map<int,int> &id_map)
{
    int id;
    if (info.count(name) > 0) {
        id = std::stoi(info[name]);
    } else {
        id = 1;
    }

    // Find the next available ID:
    while (id_map.count(id) > 0) id++;

    // Save the id in the map:
    id_map[id] = id;

    return id;
}

void display_progress(float progress)
{
    int bar_width = 70;
    std::cout << "[";
    int pos = bar_width * progress;
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}

std::string get_sha(std::string &path) {
    std::string cd_cmd = "cd " + path + " && ";
    std::string sha_cmd = cd_cmd + "git rev-parse HEAD | tr -d '\n'";
    std::string status_cmd = cd_cmd + "git status --porcelain | wc -l";

    FILE *sha_file = popen(sha_cmd.c_str(), "r");
    FILE *status_file = popen(status_cmd.c_str(), "r");

    char sha[41], status[3];
    bool success = 
        fgets(sha, 40, sha_file) != NULL &&
        fgets(status, 2, status_file) != NULL;

    pclose(sha_file);
    pclose(status_file);

    if (!success) {
        return "";
    } else if (status[0] == '0') {
        return std::string(sha);
    } else {
        return std::string(sha) + "-dirty";
    }
}

std::string get_version() {
    const char *env = std::getenv("SCRIMMAGE_ROOT");
    if (env == NULL) {
        return std::string("");
    } else {
        std::string env_path(env);
        return get_sha(env_path);
    }
}

void filter_line(int downsampling_factor,
    int num_points,
    std::vector<Eigen::Vector3d> &path,
    std::vector<Eigen::Vector3d> &filtered_path) {

    int curvature_sz = path.size() / downsampling_factor;
    std::list<std::pair<int, double>> curvature;

    auto idx = [=](int i) {return downsampling_factor * i;};
    for (int i = 1; i < curvature_sz - 1; i++) {
        Eigen::Vector3d &pt_prev = path[idx(i - 1)];
        Eigen::Vector3d &pt = path[idx(i)];
        Eigen::Vector3d &pt_next = path[idx(i + 1)];

        double curv = (pt_next - 2 * pt + pt_prev).squaredNorm();
        curvature.push_back(std::make_pair(idx(i), curv));
    }

    using Pair = std::pair<int, double>;
    curvature.sort([](Pair &a, Pair &b) {return a.second > b.second;});
    curvature.erase(std::next(curvature.begin(), num_points), curvature.end());
    curvature.sort([](Pair &a, Pair &b) {return a.first < b.first;});

    filtered_path.clear();
    filtered_path.reserve(curvature.size() + 2);
    filtered_path.push_back(path[0]);
    
    for (Pair &p : curvature) {
        filtered_path.push_back(path[p.first]);
    }

    filtered_path.push_back(path.back());
}

std::string generate_chars(std::string symbol, int num)
{
    std::string out = "";
    for (int i = 0; i < num; i++) {
        out += symbol;
    }
    return out;
}

}
