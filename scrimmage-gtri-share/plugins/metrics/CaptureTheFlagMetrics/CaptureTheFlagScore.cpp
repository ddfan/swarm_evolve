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

#include <scrimmage/parse/ConfigParse.h>
#include <scrimmage/parse/ParseUtils.h>

#include "CaptureTheFlagScore.h"

namespace sc = scrimmage;

CaptureTheFlagScore::CaptureTheFlagScore() 
{
    air_to_air_kills_ = 0;
    enemy_base_collisions_ = 0;
    distance_from_base_ = 0.0;
}

bool CaptureTheFlagScore::set_weights(std::map<std::string,std::string> &params) 
{
    air_to_air_kill_w_ = sc::get<double>("air_to_air_kill_w", params, 0.0);
    enemy_base_collision_w_ = sc::get<double>("enemy_base_collision_w", params, 0.0);
    distance_from_base_w_ = sc::get<double>("distance_from_base_w", params, 0.0);
    return true;
}

double CaptureTheFlagScore::score() {
    double s = air_to_air_kills() * air_to_air_kill_w_
        + enemy_base_collisions() * enemy_base_collision_w_
        + distance_from_base() * distance_from_base_w_;
    return s;
}
