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

class CaptureTheFlagScore {
 public:
    CaptureTheFlagScore();

    bool set_weights(std::map<std::string,std::string> &params);
    
    void increment_air_to_air_kills() { air_to_air_kills_++; }
    void increment_enemy_base_collisions() { enemy_base_collisions_++; }
        
    void add_enemy_kills(int kills) { air_to_air_kills_ += kills; }
    void add_enemy_base_collisions(int c) { enemy_base_collisions_ += c; }
    void add_distance_from_base(double c) { distance_from_base_ += c; }
    
    int air_to_air_kills() { return air_to_air_kills_; }
    int enemy_base_collisions() { return enemy_base_collisions_; }
    double distance_from_base() { return distance_from_base_; }

    void set_air_to_air_kills(int ata_kills) {air_to_air_kills_ = ata_kills;}
    void set_enemy_base_collisions(int ebh_collisions) {enemy_base_collisions_ = ebh_collisions;}
    void set_add_distance_from_base(double c) { distance_from_base_ = c; }

    double score();

protected:
    // 1.) Air-to-Air (AtA)
    // 2.) Enemy Base Hits (EBH)
    int air_to_air_kills_;
    int enemy_base_collisions_;
    double distance_from_base_;
    
    double air_to_air_kill_w_;
    double enemy_base_collision_w_;
    double distance_from_base_w_;
};

#endif
