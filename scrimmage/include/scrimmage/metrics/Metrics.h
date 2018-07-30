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
#ifndef METRICS_H_
#define METRICS_H_
#include <map>
#include <unordered_map>
#include <list>

#include <scrimmage/plugin_manager/Plugin.h>


namespace scrimmage {

class Metrics : public Plugin{                
 public:
    
    Metrics();
    virtual ~Metrics();

    virtual std::string name();
    virtual void init();
    virtual void init(std::map<std::string,std::string> &params);
    virtual bool step_metrics(double t, double dt);
    
    void set_team_lookup(std::shared_ptr<std::unordered_map<int,int> > &lookup);

    virtual void calc_team_scores();
    virtual void print_team_summaries();
    
    // Key 1  : Team ID
    // Value 1: Map of..
    //                   Key 2  : Header string
    //                   Value 2: Metric value 
    virtual std::map<int, std::map<std::string, double>> &team_metrics();

    virtual std::list<std::string> &headers();

    virtual std::map<int, double> & team_scores();
            
 protected:
    std::string weights_file_;
    std::shared_ptr<std::unordered_map<int,int> > team_lookup_;
    std::map<int, std::map<std::string, double>> team_metrics_;
    std::map<int, double> team_scores_;
    std::list<std::string> headers_;
    
private:
};

using MetricsPtr = std::shared_ptr<Metrics>;

} // namespace scrimmage
#endif
