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
#include <iomanip>
#include <chrono>
#include <ctime>
#include <fstream>

#include <string>
#include <sstream>
#include <cstdlib>
#include <scrimmage/parse/MissionParse.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/log/Log.h>
#include <scrimmage/metrics/Metrics.h>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
namespace sc = scrimmage;

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cout << "usage: " << argv[0] << " <directory of filter results>" << endl;
        return -1;
    }

    // Directory holding all the runs (typically, ~/scrimmage-log)
    std::string dir = std::string(argv[1]);

    // Find all .txt files under the directory
    std::vector<std::string> paths;
    fs::path root = dir;
    std::string ext = ".result";
    if(fs::exists(root) && fs::is_directory(root)) {
        fs::recursive_directory_iterator it(root);
        fs::recursive_directory_iterator endit;

        while(it != endit) {
            if(fs::is_regular_file(*it) && it->path().extension() == ext) {
                std::string full_path = fs::absolute(it->path()).string();
                paths.push_back(full_path);
            }
            ++it;
        }
    } else {
        cout << "Path doesn't exist: " << dir << endl;
    }

    // Key: Name of file stem
    // Value: List of paths of this type
    std::map<std::string, std::list<std::string> > scenarios;

    // Open each .txt file and extract a metric
    for (std::vector<std::string>::iterator it = paths.begin();
         it != paths.end(); it++) {

        std::string filename = *it;        

        if (!fs::exists(fs::path(filename))) {
            cout << "Filter file doesn't exist: " << filename << endl;
            return -1;
        }

        std::string stem = fs::path(filename).stem().string();

        // Open the type file and record each directory name
        std::ifstream file(filename);
        if (!file.is_open()) {
            cout << "Failed to open file: " << filename << endl;
            return -1;
        }

        std::string line;
        while (getline(file,line)) {
            scenarios[stem].push_back(line);
        }
        file.close();
    }

    int col_wid = 16;
    std::vector<std::string> headings;
    headings.push_back("Number");
    headings.push_back("Name");
    headings.push_back("Count");
        
    std::map<int,std::string> name_2_index;
    std::string choose_type;
    int choose_num = -1;
    do {
        cout << "====================================================" << endl;
        cout << "Choose an outcome number: " << endl;
        cout << "----------------------------------------------------" << endl;
        for (auto &i : headings) {
            cout << std::left << std::setw(col_wid) << i;
        }
        cout << endl;                    
        cout << "----------------------------------------------------" << endl;
        int i = 0;
        for (std::map<std::string, std::list<std::string> >::iterator it = scenarios.begin();
             it != scenarios.end(); it++) {
            name_2_index[i] = it->first;
            std::string select_str = "[" + std::to_string(i) + "]";
            cout << std::left << std::setw(col_wid) << select_str;
            cout << std::left << std::setw(col_wid) << it->first;
            cout << std::left << std::setw(col_wid) << it->second.size() << endl;
            i++;            
        }
        cout << ">> ";
        std::cin >> choose_type;
        choose_num = std::stoi(choose_type);
    } while (name_2_index.count(choose_num) == 0);    
    
    cout << "Playing back: " << name_2_index[choose_num] << endl;    

    for (std::list<std::string>::iterator it = scenarios[name_2_index[choose_num]].begin();
         it != scenarios[name_2_index[choose_num]].end(); /* no inc */) {

        cout << "Mission: " << *it << endl;
        
        // Show the run in the visualizer
        std::string cmd = "scrimmage-playback " + *it;
        int status = std::system(cmd.c_str());
        if (status != 0) {
            cout << "Playback failed." << endl;
        }

        cout << "====================================" << endl;
        cout << "Choose an option: " << endl;
        cout << "(r)eplay" << endl;
        cout << "(n)ext" << endl;
        cout << "(q)uit" << endl;
        cout << "$ ";
        std::string choose;
        std::cin >> choose;

        if (choose == "q") {
            break;
        }

        if (choose == "r") {
            // Don't increment, replay
        } else if (choose == "n") {
            it++;
        }
    }
    cout << "Complete." << endl;
    return 0;
}
