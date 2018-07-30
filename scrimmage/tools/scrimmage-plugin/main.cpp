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
#include <string>
#include <memory>
#include <scrimmage/parse/ConfigParse.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using std::cout;
using std::endl;

namespace fs = boost::filesystem;
namespace po = boost::program_options;
namespace sc = scrimmage;

int main(int argc, char *argv[])
{
    // Declare the supported options.
    po::options_description desc("Allowed options");    
    desc.add_options()
        ("help,h", "produce help message")
        ("plugin-name,p", po::value<std::string>(), "the plugin name")
        ;
    
    // Immplement positional argument
    po::positional_options_description p;
    p.add("plugin-name", -1);       

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(p).run(), vm);
    po::notify(vm);    

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    std::string plugin_type = "autonomy";
    std::map<std::string, std::string> overrides;
    std::string plugin_name_xml;
    if (vm.count("plugin-name")) {
        plugin_name_xml = vm["plugin-name"].as<std::string>();
    } else {        
        cout << "plugin-name was not set.\n";
        return -1;
    }
    
    sc::FileSearch file_search;
    sc::ConfigParse config_parse;
    config_parse.set_required("library");
    if (!config_parse.parse(overrides, plugin_name_xml, "SCRIMMAGE_PLUGIN_PATH", file_search)) {
        std::cout << "Failed to parse: " << plugin_name_xml << " for type " << plugin_type << std::endl;
        return -2;
    }
    cout << "==========================================" << endl;
    cout << "Plugin found." << endl;    
    cout << "Name: " << plugin_name_xml << endl;
    cout << "File: " << config_parse.filename() << endl;
    cout << "Library: " << config_parse.params()["library"] << endl;
    cout << "-------------------------" << endl;
    cout << "Params: ";
    config_parse.print_params();
    
    return 0;
}
