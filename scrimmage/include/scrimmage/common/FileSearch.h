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
#ifndef FILE_SEARCH_H_
#define FILE_SEARCH_H_

#include <unordered_map>
#include <list>
#include <memory>

namespace scrimmage {

class FileSearch {
 public:
    void clear();
    bool find_file(const std::string &filename, std::string ext, std::string env_var, std::string &result);
    void find_files(std::string env_var, const std::string &ext,
                    std::unordered_map<std::string, std::list<std::string>> &out);

 protected:
    // cache_[env_var][ext][filename] = list of full paths to files with that filename
    std::unordered_map<std::string,
        std::unordered_map<std::string, 
            std::unordered_map<std::string, std::list<std::string>>>> cache_;
};

} // namespace scrimmage

#endif // FILE_SEARCH_H_
