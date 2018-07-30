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
#include <scrimmage/common/RTree.h>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

namespace scrimmage {

RTree::RTree() {}

void RTree::init(int size)
{
    if (size > 0) {
        rtree_ = std::make_shared<rtree_t>(bgi::dynamic_rstar(size));
        size_ = size;
    }
}

void RTree::clear() {
    rtree_->clear(); 
    rtree_team_.clear();
}

void RTree::add(Eigen::Vector3d &pos, ID &id) {
    point p(pos(0), pos(1), pos(2));
    std::pair<point, ID> pair(p, id);
    rtree_->insert(pair);

    int team_id = id.team_id();
    auto it = rtree_team_.find(team_id);
    if (it == rtree_team_.end()) {
        rtreePtr rtree = std::make_shared<rtree_t>(bgi::dynamic_rstar(size_));
        it = rtree_team_.insert(std::make_pair(team_id, rtree)).first;
    }
    it->second->insert(pair);
}

void results_to_neighbors(std::list<point_id_t> &results,
                          std::vector<ID> &neighbors,
                          int self_id) {
    neighbors.clear();
    neighbors.reserve(results.size());
    if (self_id >= 0) {
        for (point_id_t &pt_id : results) {
            if (pt_id.second.id() != self_id) {
                neighbors.push_back(pt_id.second);
            }
        }
    } else {
        for (point_id_t &pt_id : results) {
            neighbors.push_back(pt_id.second);
        }
    }
}

void RTree::nearest_n_neighbors(const Eigen::Vector3d &pos,
                                std::vector<ID> &neighbors, unsigned int n,
                                int self_id, int team_id)
{
    std::list<point_id_t> results;
    point sought(pos(0), pos(1), pos(2));

    if (self_id != -1) {
        // assume that if an id is given then it will be located at pos so
        // would always be in the neighborhood
        n += 1;
    }

    if (team_id == -1) {
        rtree_->query(bgi::nearest(sought, n), std::front_inserter(results));
    } else {
        auto it = rtree_team_.find(team_id);
        if (it == rtree_team_.end()) {
            neighbors.clear();
            return;
        } else {
            it->second->query(bgi::nearest(sought, n), std::front_inserter(results));
        }
    }
    results_to_neighbors(results, neighbors, self_id);
}

void RTree::neighbors_in_range(const Eigen::Vector3d &pos,
                               std::vector<ID> &neighbors,
                               double dist,
                               int self_id, int team_id) {
    // see here: http://stackoverflow.com/a/22910447
    std::list<point_id_t> results;
    double x = pos(0);
    double y = pos(1);
    double z = pos(2);
    point sought(x, y, z);

    bg::model::box<point> box(
        point(x - dist, y - dist, z - dist), point(x + dist, y + dist, z + dist)
    );

    auto dist_func = [&](point_id_t const& v) {return bg::distance(v.first, sought) < dist;};

    if (team_id == -1) {
        rtree_->query(
            bgi::within(box) && bgi::satisfies(dist_func),
            std::front_inserter(results)
        );
    } else {
        auto it = rtree_team_.find(team_id);
        if (it == rtree_team_.end()) {
            neighbors.clear();
            return;
        } else {
            it->second->query(
                bgi::within(box) && bgi::satisfies(dist_func),
                std::front_inserter(results)
            );
        }
    }

    results_to_neighbors(results, neighbors, self_id);
}

} // namespace scrimmage
