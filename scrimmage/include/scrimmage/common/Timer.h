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
#ifndef TIMER_H_
#define TIMER_H_

#include <boost/date_time/posix_time/posix_time.hpp>

namespace scrimmage {

class Timer {
public:
    void start_overall_timer();

    boost::posix_time::time_duration elapsed_time();

    void start_loop_timer();

    bool loop_wait();

    void set_iterate_rate(double iterate_rate);

    void set_time_warp(double time_warp);

    void update_time_config();

    static unsigned long getnanotime();

    void inc_warp();

    void dec_warp();

    double time_warp();

protected:
    double time_warp_;
    boost::posix_time::ptime start_time_;

    boost::posix_time::ptime actual_time_;
    boost::posix_time::time_duration actual_elapsed_time_;

    boost::posix_time::ptime sim_time_;
    boost::posix_time::time_duration sim_elapsed_time_;

    boost::posix_time::ptime loop_timer_;
    boost::posix_time::time_duration iterate_period_;
    double iterate_rate_;

private:
};
}
#endif
