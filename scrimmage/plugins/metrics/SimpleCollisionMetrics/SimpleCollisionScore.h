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
#ifndef SCORE_H_
#define SCORE_H_

#include <string>

class SimpleCollisionScore {
 public:
    SimpleCollisionScore();

    bool set_weights(std::map<std::string,std::string> &params);
    
    void set_flight_time_start(double t) { flight_time_start_ = t; }
    void set_flight_time_end(double t) { flight_time_end_ = t; }
    void increment_non_team_collisions() { non_team_collisions_++; }
    void increment_team_collisions() { team_collisions_++; }
    void increment_ground_collisions() { ground_collisions_++; }

    void add_non_team_collisions(int c) { non_team_collisions_ += c; }
    void add_team_collisions(int c) { team_collisions_ += c; }
    void add_ground_collisions(int c) { ground_collisions_ += c; }
    void add_flight_time(double t) { flight_time_end_ += t; }

    int non_team_collisions() { return non_team_collisions_; }
    int team_collisions() { return team_collisions_; }
    int ground_collisions() { return ground_collisions_; }

    void set_max_flight_time(double t) { max_flight_time_ = t; }
    void increment_entity_count() { entity_count_++; }

    int entity_count() { return entity_count_; }
    void set_entity_count(int entity_count) {entity_count_ = entity_count;}
    void set_non_team_collisions(int non_team_collisions) {non_team_collisions_ = non_team_collisions;}
    void set_team_collisions(int team_collisions) {team_collisions_ = team_collisions;}
    void set_ground_collisions(int ground_collisions) {ground_collisions_ = ground_collisions;}
    
    double flight_time() {return (flight_time_end_ - flight_time_start_);}
    double flight_time_end() {return flight_time_end_;}
    double flight_time_start() {return flight_time_start_;}
    double flight_time_norm() {return (flight_time() / max_flight_time_);}

    double score();

protected:
    // 1.) Flight Time (FT)
    double flight_time_start_;
    double flight_time_end_;
    int non_team_collisions_;
    int team_collisions_;
    int ground_collisions_;

    double flight_time_w_;
    double non_team_collisions_w_;
    double team_collisions_w_;
    double ground_collisions_w_;
    
    double max_flight_time_;
    int entity_count_;
};

#endif
