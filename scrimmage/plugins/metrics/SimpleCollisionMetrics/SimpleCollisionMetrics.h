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
#ifndef SimpleCollisionMetrics_H_
#define SimpleCollisionMetrics_H_
#include <scrimmage/metrics/Metrics.h>

#include "SimpleCollisionScore.h"

namespace sc = scrimmage;

class SimpleCollisionMetrics : public scrimmage::Metrics {
public:
    SimpleCollisionMetrics();

    virtual std::string name() { return std::string("SimpleCollisionMetrics"); }
    virtual void init(std::map<std::string,std::string> &params);
    virtual bool step_metrics(double t, double dt);
    virtual void calc_team_scores();
    virtual void print_team_summaries();    
    
protected:
    sc::SubscriberPtr sub_team_collision_;
    sc::SubscriberPtr sub_non_team_collision_;
    sc::SubscriberPtr sub_ground_collision_;
    sc::SubscriberPtr sub_ent_gen_;
    sc::SubscriberPtr sub_ent_rm_;
    sc::SubscriberPtr sub_ent_pres_end_;

    std::map<int, SimpleCollisionScore> scores_;
    std::map<int, SimpleCollisionScore> team_coll_scores_;
    std::map<int, bool> surviving_teams_;

    std::map<std::string,std::string> params_;

private:
};

#endif
