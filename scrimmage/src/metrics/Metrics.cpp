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

#include <scrimmage/metrics/Metrics.h>
#include <scrimmage/proto/Frame.pb.h>
#include <limits>

using std::cout;
using std::endl;

namespace scrimmage {
    
Metrics::Metrics() {}

Metrics::~Metrics() {}

std::string Metrics::name() { return std::string("Metrics"); }

void Metrics::init() {}

void Metrics::init(std::map<std::string, std::string> &params) {}

bool Metrics::step_metrics(double t, double dt) { return false; }

void Metrics::set_team_lookup(std::shared_ptr<std::unordered_map<int, int> > &lookup)
{ team_lookup_ = lookup; }

void Metrics::calc_team_scores() {}

void Metrics::print_team_summaries() {}

std::map<int, std::map<std::string, double> > &Metrics::team_metrics()
{ return team_metrics_; }

std::list<std::string> &Metrics::headers()
{ return headers_; }

std::map<int, double> &Metrics::team_scores()
{ return team_scores_; }

} // namespace scrimmage
