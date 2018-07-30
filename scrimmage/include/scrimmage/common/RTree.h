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
#ifndef RTREE_H_
#define RTREE_H_

#include <Eigen/Dense>
#include <map>
#include <vector>
#include <scrimmage/common/ID.h>
#include <memory>
#include <boost/tuple/tuple.hpp>
#include <boost/geometry/index/detail/exception.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/index/parameters.hpp> // for dynamic_rstar definition
#include <boost/geometry/geometries/point.hpp> // for model::point
#include <boost/geometry/index/indexable.hpp>

namespace boost { namespace geometry { namespace index {
    // boost/geometry/index/rtree.hpp
    template <typename T1, typename T2, typename T3, typename T4, typename T5> class rtree;
}}}

namespace scrimmage {

typedef boost::geometry::model::point<double, 3, boost::geometry::cs::cartesian> point;
typedef std::pair<point, ID> point_id_t;
typedef boost::geometry::index::rtree<
    point_id_t,
    boost::geometry::index::dynamic_rstar,
    boost::geometry::index::indexable<point_id_t>,
    std::equal_to<point_id_t>,
    std::allocator<point_id_t>> rtree_t;

typedef std::shared_ptr<rtree_t> rtreePtr;

class RTree {
 public:
    RTree();
    void init(int size);
    void clear();

    void add(Eigen::Vector3d &pos, ID &id);
    void nearest_n_neighbors(const Eigen::Vector3d &pos,
                             std::vector<ID> &neighbors, unsigned int n,
                             int self_id=-1, int team_id=-1);
    void neighbors_in_range(const Eigen::Vector3d &pos,
                            std::vector<ID> &neighbors, double dist,
                            int self_id=-1, int team_id=-1);
 protected:

    rtreePtr rtree_;
    std::map<int, rtreePtr> rtree_team_;
    int size_;

 private:
};

typedef std::shared_ptr<RTree> RTreePtr;
}  // namespace scrimmage

#endif
