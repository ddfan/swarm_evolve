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

#include "SimpleCollisionScore.h"

namespace sc = scrimmage;

SimpleCollisionScore::SimpleCollisionScore() {
    flight_time_start_ = 0;
    flight_time_end_ = 0;
    non_team_collisions_ = 0;
    team_collisions_ = 0;
    ground_collisions_ = 0;
    entity_count_ = 0;
}

bool SimpleCollisionScore::set_weights(std::map<std::string,std::string> &params) {
    flight_time_w_ = sc::get<double>("flight_time_w", params, 0.0);
    team_collisions_w_ = sc::get<double>("team_collisions_w", params, 0.0);
    ground_collisions_w_ = sc::get<double>("ground_collisions_w", params, 0.0);
    non_team_collisions_w_ = sc::get<double>("non_team_collisions_w", params, 0.0);    

    return true;
}

double SimpleCollisionScore::score() {
    double s = flight_time_norm() * flight_time_w_
        + non_team_collisions() * non_team_collisions_w_
        + team_collisions() * team_collisions_w_
        + ground_collisions() * ground_collisions_w_;
    return s;
}
