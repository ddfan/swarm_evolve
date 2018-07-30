/*
 * ---------------------------------------------------------------------------
 * @section LICENSE
 *  
 * Copyright (c) 2016 Georgia Tech Research Institute (GTRI) 
 *               All Rights Reserved
 *  
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 * ---------------------------------------------------------------------------
 * @file filename.ext
 * @author Kevin DeMarco <kevin.demarco@gtri.gatech.edu> 
 * @author Eric Squires <eric.squires@gtri.gatech.edu>
 * @version 1.0
 * ---------------------------------------------------------------------------
 * @brief A brief description.
 * 
 * @section DESCRIPTION
 * A long description.
 * ---------------------------------------------------------------------------
 */
#ifndef MOTIONMODEL_H_
#define MOTIONMODEL_H_
#include <map>
#include <vector>

#include <Eigen/Dense>

#include <scrimmage/fwd_decl.h>
#include <scrimmage/plugin_manager/Plugin.h>

namespace scrimmage {

class MotionModel : public Plugin {
 public:
    typedef std::vector<double> vector_t;

    MotionModel();
    std::string type();

    virtual bool init(std::map<std::string, std::string> &info,
                      std::map<std::string, std::string> &params);

    virtual bool step(double time, double dt);
    virtual bool posthumous(double t);
    virtual StatePtr &state();
    virtual void set_state(StatePtr &state);
    virtual void teleport(StatePtr &state);
    virtual void set_external_force(Eigen::Vector3d force);
    virtual void set_mass(double mass) { mass_ = mass; }
    virtual double mass() { return mass_; }
    
 protected:
    
    void ode_step(double dt);
    virtual void model(const vector_t &x , vector_t &dxdt , double t);

    StatePtr state_;
    vector_t x_;
    vector_t u_;

    Eigen::Vector3d ext_force_;
    double mass_;
};

using MotionModelPtr = std::shared_ptr<MotionModel>;
}
#endif
