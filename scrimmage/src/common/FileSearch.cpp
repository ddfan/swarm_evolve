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
#include <scrimmage/common/FileSearch.h>
#include <iostream>
#include <unordered_set>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

namespace fs = ::boost::filesystem;

namespace scrimmage {

void FileSearch::clear() {cache_.clear();}

bool FileSearch::find_file(const std::string &search, std::string ext, std::string env_var, std::string &result)
{
    // Find the xml file.
    // Search order:
    // 1. search could be the full path
    // 2. search could be just the name of the plugin (e.g., QuadTest_plugin)
    //    a. Is the file located in the plugin path environment variable?

    // Determine if the passed in file exists (could be full path)        
    std::list<std::string> filenames;
    if (!ext.empty() && ext[0] != '.') {
        ext = std::string(".") + ext;
    }
    std::string search_filename = search + ext;

    if ( !fs::exists( search ) ) {
        // files[search_filename] = list of full paths
        std::unordered_map<std::string, std::list<std::string>> files;
        find_files(env_var, ext, files);
        filenames = files[search_filename];
    } else {
        filenames.push_back(search);
    }
    
    if (filenames.empty()) {
        //std::cout << "Failed to find xml filename: " << search << std::endl;
        return false;
    }

    // Use the last XML file that was found
    result = filenames.back();
    
    if (filenames.size() > 1) {
        std::cout << "===============================================" << std::endl;
        std::cout << "WARNING: Multiple XML files with same name found" << std::endl;            
        for (std::string &full_path : filenames) {
            std::cout << full_path << std::endl;
        }
        std::cout << "Using XML file at: " << result << std::endl; 
        std::cout << "===============================================" << std::endl;
    }
    return true;
}

// Give an environment variable and a file extension:
// Find the absolute path to all files in environment variable paths
void FileSearch::find_files(std::string env_var, const std::string &ext,
                            std::unordered_map<std::string, std::list<std::string>> &out)
{
    auto cache_it = cache_.find(env_var);
    auto ext_it = cache_[env_var].find(ext);
    if (cache_it != cache_.end()) {
        if (ext_it != cache_it->second.end()) {
            out = ext_it->second;
            return;
        } else {
            cache_it->second[ext];
            ext_it = cache_it->second.find(ext);
        }
    } else {
        cache_[env_var][ext];
        ext_it = cache_[env_var].find(ext);
    }

    out.clear();

    // Get the environment variable
    const char* env_p = std::getenv(env_var.c_str());
    if (env_p == NULL) {
        std::cout << env_var << " environment variable not set" << std::endl;
        return;
    }

    std::string env_path = std::string(env_p);

    // Tokenize the path and loop through each directory
    boost::char_separator<char> sep(":");
    boost::tokenizer< boost::char_separator<char> > tokens(env_path, sep);
    std::unordered_set<std::string> dirs_checked;

    BOOST_FOREACH (const std::string& t, tokens) {
        // Search for all files in the current directory with
        // the extension
        fs::path root = t;

        if (dirs_checked.count(t) > 0) {
            // pass
        } else if (fs::exists(root) && fs::is_directory(root)) {
            dirs_checked.insert(t);

            fs::recursive_directory_iterator it(root);
            fs::recursive_directory_iterator endit;

            while(it != endit) {
                fs::path path = it->path();
                if(fs::is_regular_file(*it) && path.extension() == ext) {
                    std::string fname = path.filename().string();
                    std::string full_path = fs::absolute(path).string();
                    ext_it->second[fname].push_back(full_path);
                }
                ++it;
            }
        } else {
            std::cout << "Search path doesn't exist: " << t << std::endl;
        }
    }

    out = ext_it->second;
    return;
}

} // namespace scrimmage
