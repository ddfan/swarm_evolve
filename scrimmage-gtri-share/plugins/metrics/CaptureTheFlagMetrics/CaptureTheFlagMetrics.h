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
#ifndef CAPTURETHEFLAGMETRICS_H_
#define CAPTURETHEFLAGMETRICS_H_
#include <map>
#include <unordered_map>
#include <list>
#include <scrimmage/metrics/Metrics.h>

#include "CaptureTheFlagScore.h"

class CaptureTheFlagMetrics : public scrimmage::Metrics {
 public:
    
    CaptureTheFlagMetrics();

    virtual std::string name() { return std::string("CaptureTheFlagMetrics"); }
    virtual void init(std::map<std::string,std::string> &params);
    virtual bool step_metrics(double t, double dt);
    virtual void calc_team_scores();
    virtual void print_team_summaries();    
    std::map<int, CaptureTheFlagScore> &scores() {return scores_;}
    
protected:
    std::map<std::string,std::string> params_;

    std::map<int, CaptureTheFlagScore> scores_; // Key: Individual ID , Item: Score
    std::map<int, CaptureTheFlagScore> ctf_scores_;    

    scrimmage::SubscriberPtr sub_base_coll_;
    scrimmage::SubscriberPtr sub_fire_result_;
    scrimmage::SubscriberPtr sub_distfrombase_;
private:
};

#endif
