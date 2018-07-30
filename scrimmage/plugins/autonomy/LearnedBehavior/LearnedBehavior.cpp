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

#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/entity/Entity.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <scrimmage/math/State.h>
#include <scrimmage/common/RTree.h>
#include <scrimmage/math/Angles.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>


#include "LearnedBehavior.h"

using std::cout;
using std::endl;

namespace sc = scrimmage;

REGISTER_PLUGIN(scrimmage::Autonomy, LearnedBehavior, LearnedBehavior_plugin)

LearnedBehavior::LearnedBehavior()
{     
}

void LearnedBehavior::init(std::map<std::string,std::string> &params)
{
    desired_state_->vel() << 30, 0, 0;
    desired_state_->quat().set(0, 0, state_->quat().yaw());
    desired_state_->pos() << 0, 0, state_->pos()(2);    

    desired_vel = scrimmage::get("desired_velocity", params, 30);
    desired_state_->vel() << desired_vel, 0, 0;

    if (parent_.lock()->parameter_vector().size()>0){
        w_align_=parent_.lock()->parameter_vector()[0];
        w_avoid_=parent_.lock()->parameter_vector()[1];
        w_centroid_=parent_.lock()->parameter_vector()[2];
    }else{
        w_align_ = scrimmage::get("align_weight", params, 0.01);
        w_avoid_ = scrimmage::get("avoid_weight", params, 0.95);
        w_centroid_ = scrimmage::get("centroid_weight", params, 0.05);
    }

    //normalize the values to sum to 1
//    double w_sum=w_align_+w_avoid_+w_centroid_;
//    w_align_=w_align_/w_sum;
//    w_avoid_=w_avoid_/w_sum;
//    w_centroid_=w_centroid_/w_sum;

}

bool LearnedBehavior::step_autonomy(double t, double dt)
{
//    action_.clear(); // reset the action every step

    // Find n nearest neighbor of ownship
    int num_neighbors = 7;
    std::vector<scrimmage::ID> rtree_neighbors;
    rtree_->nearest_n_neighbors(state_->pos_const(), rtree_neighbors, num_neighbors);

//    {
//        // Ensure that the neighbors are "in front" (abs(bearing) < 100)
//        // Remove contacts that are behind
//        double bearing = scrimmage::Angles::deg2rad(100.0*2.0);
//        std::vector<scrimmage::ID>::iterator it = rtree_neighbors.begin();
//        while (it != rtree_neighbors.end()) {
//            if (state_->InFieldOfView(*(*contacts_)[it->id()].state(),bearing,bearing)) {
//                // The neighbor is "in front"
//                it++;
//            } else {
//                // The neighbor is "behind." Remove it.
//                it = rtree_neighbors.erase(it);
//            }
//        }
//    }

    // Steer to avoid local neighbors
    // Align with neighbors
    // Cohesion: move towards average position of neighbors
    // (i.e., Find centroid of neighbors)
    Eigen::Vector3d avoid = Eigen::Vector3d::Zero();
    Eigen::Vector3d align = Eigen::Vector3d::Zero();
    Eigen::Vector3d centroid = Eigen::Vector3d::Zero();
    int neighbor_count = 0;
    for (std::vector<scrimmage::ID>::iterator it = rtree_neighbors.begin();
         it != rtree_neighbors.end(); it++) {

        if (it->id() == parent_.lock()->id().id()) {
            continue;
        }
//        std::cout<<(*contacts_)[it->id()].state()->pos()(0)<<","<<(*contacts_)[it->id()].state()->pos()(1)<<","<<(*contacts_)[it->id()].state()->pos()(2)<<","<<it->id()<<", n";

        centroid = centroid + (*contacts_)[it->id()].state()->pos();

        align = align + (*contacts_)[it->id()].state()->pos() + desired_vel*(*contacts_)[it->id()].state()->vel();

        double dist = (state_->pos() - (*contacts_)[it->id()].state()->pos()).norm();
        avoid = avoid + (state_->pos() - (*contacts_)[it->id()].state()->pos()) / pow(dist,2);

        neighbor_count++;
    }

//    std::cout<<"\n";

    if (neighbor_count == 0)
        centroid=Eigen::Vector3d::Zero();
    else
        centroid = centroid / (double)neighbor_count;
    avoid = state_->pos() + avoid;

    if (rtree_neighbors.size() > 0) {
          
//          std::cout << "Centroid: " << centroid << "\n";
//          std::cout << "Align: " << align << "\n";
//          std::cout << "Avoid: " << avoid << "\n";
//        std::cout<<avoid(0)<<","<<centroid(0)<<","<<align(0)<<"\n";
        Eigen::Vector3d target = avoid * w_avoid_ + centroid * w_centroid_ + align * w_align_;
        Eigen::Vector3d diff = target - state_->pos();
        Eigen::Vector3d v = desired_vel * diff.normalized();
        
        ///////////////////////////////////////////////////////////////////////////
        // Convert desired velocity to desired speed, heading, and pitch controls
        ///////////////////////////////////////////////////////////////////////////
        desired_state_->vel()(0) = v.norm();

        // Desired heading
        double heading = scrimmage::Angles::angle_2pi(atan2(v(1), v(0)));

        // Desired pitch
        Eigen::Vector2d xy(v(0), v(1));
        double pitch = scrimmage::Angles::angle_2pi(atan2(v(2), xy.norm()));

        // Set Desired Altitude to goal's z-position
        desired_state_->pos() = target;

        // Set the desired pitch and heading
        desired_state_->quat().set(0, pitch, heading);
    }

    return true;
}
