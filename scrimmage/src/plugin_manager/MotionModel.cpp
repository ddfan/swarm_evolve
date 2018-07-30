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
#include <scrimmage/motion/MotionModel.h>
#include <functional>
#include <boost/numeric/odeint.hpp>

namespace pl = std::placeholders;

namespace scrimmage {

MotionModel::MotionModel() : mass_(1.0) {}

std::string MotionModel::type() { return std::string("MotionModel"); }

bool MotionModel::init(std::map<std::string, std::string> &info, std::map<std::string, std::string> &params)
{ return false; }

bool MotionModel::step(double time, double dt) { return false; }

bool MotionModel::posthumous(double t) { return true; }

StatePtr &MotionModel::state() {return state_;}

void MotionModel::set_state(StatePtr &state) {state_ = state;}

void MotionModel::teleport(StatePtr &state) {state_ = state;}

void MotionModel::ode_step(double dt) {
    auto sys = std::bind(&MotionModel::model, this, pl::_1, pl::_2, pl::_3);
    boost::numeric::odeint::runge_kutta4<std::vector<double>> stepper;
    stepper.do_step(sys, x_, 0, dt);
}

void MotionModel::model(const MotionModel::vector_t &x, MotionModel::vector_t &dxdt, double t) {}

void MotionModel::set_external_force(Eigen::Vector3d force)
{
    ext_force_ = force;
}
    
}
