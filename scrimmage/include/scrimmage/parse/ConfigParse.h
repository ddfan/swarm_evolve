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
#ifndef CONFIGPARSE_H_
#define CONFIGPARSE_H_
#include <map>
#include <vector>
#include <scrimmage/common/FileSearch.h>

namespace scrimmage {
    class ConfigParse {
    public:
        ConfigParse();
        void set_required(std::string node_name);
        bool parse(std::map<std::string, std::string> &overrides, std::string filename, std::string env_var, FileSearch &file_search);
        std::map<std::string, std::string> & params();
        std::string filename();
        std::string directory();
        std::string extension();
        std::string stem();
        void print_params();
        
    protected:
        std::map<std::string, std::string> params_;
        std::vector<std::string> required_;
        std::string filename_;
    private:
    };
}
#endif
