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

#include <string>
#include <sstream>
#include <cstdlib>
#include <scrimmage/parse/MissionParse.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/log/Log.h>
#include <scrimmage/metrics/Metrics.h>
#include <scrimmage/common/FileSearch.h>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

using std::cout;
using std::endl;

void usage(char *argv[])
{
    cout << endl << "Usage: " << argv[0] << " -d <log-dir> -w <weights-xml>" 
         << endl << endl;
}

int main(int argc, char *argv[])
{
    cout << endl << "=============================" << endl;
    cout << "Running metrics..." << endl;
    
    std::string log_dir = "";
    std::string weights_file = "";
    int opt;
    while ((opt = getopt (argc, argv, "d:w:")) != -1) {
        switch (opt) {
        case 'd':
            log_dir = std::string(optarg);
            break;
        case 'w':
            weights_file = std::string(optarg);
            break;        
        case '?':
            if (optopt == 'd') {
                fprintf (stderr, "Option -%d requires a swarm log directory.\n", optopt);
            } else if (optopt == 'w') {
                fprintf (stderr, "Option -%d requires a weights XML file.\n", optopt);
            } else {
                fprintf (stderr,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
            }
            usage(argv);
            return 1;
        default:
            exit(EXIT_FAILURE);
        }
    }        

    if (!fs::exists(fs::path(log_dir))) {
        cout << "Log directory doesn't exist: " << log_dir << endl;        
        usage(argv);
        return -1;
    }

    std::string log_file = log_dir + "/frames.bin";    
    if (!fs::exists(fs::path(log_file))) {
        cout << "Log file doesn't exist: " << log_file << endl;
        usage(argv);
        return -1;
    }    

    if (!fs::exists(fs::path(weights_file))) {
        // It isn't a full path, is it located under the CONFIG_PATH?
        std::unordered_map<std::string, std::list<std::string>> files;
        scrimmage::FileSearch().find_files("SCRIMMAGE_CONFIG_PATH", ".xml", files);
        
        cout << "Weights file doesn't exist: " << weights_file << endl;
        usage(argv);
        return -1;
    }    

    scrimmage::Log log;
    log.parse(log_file, scrimmage::Log::FRAMES);
    cout << "Frames parsed: " << log.frames().size() << endl;

    scrimmage::Metrics metrics;
    metrics.set_weights_file(weights_file);
    metrics.process_log(log);
    metrics.print_team_counts();
    metrics.print_end_condition();
    cout << "Surviving Team: " << metrics.surviving_team() << endl;

    return 0;
}
