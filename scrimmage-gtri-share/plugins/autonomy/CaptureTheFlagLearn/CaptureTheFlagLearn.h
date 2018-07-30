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
#ifndef CaptureTheFlagLearn_H_
#define CaptureTheFlagLearn_H_
#include <memory>
#include <scrimmage/autonomy/Autonomy.h>
#include "tiny_dnn/tiny_dnn.h"


class CaptureTheFlagLearn : public scrimmage::Autonomy {
public:
    typedef enum Mode {
         GreedyShooter = 0,
         DiveBomber,
         Defender
    } Mode_t;

    CaptureTheFlagLearn();
    ~CaptureTheFlagLearn() {}
    virtual void init(std::map<std::string,std::string> &params);
    virtual bool step_autonomy(double t, double dt);
    virtual bool posthumous(double t);

protected:
private:     
    Mode_t mode_;
    double dist_xy_;
    double dive_altitude_;
    double avoid_dist_;

    double fire_range_max_;
    double fire_FOV_;
    bool fire_2D_mode_;
    double avoid_ground_height_;

    float_t sigma_;
    size_t n_friends_;
    size_t n_enemies_;

    tiny_dnn::network<tiny_dnn::sequential> policy_network;

    scrimmage::PublisherPtr pub_fire_;
};

#endif
