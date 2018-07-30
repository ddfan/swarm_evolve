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
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <scrimmage/math/State.h>
#include <Eigen/Dense>
#define _USE_MATH_DEFINES
#include <cmath>

namespace sc = scrimmage;
using Eigen::Vector3d;

TEST(test_state, local_frames) {

    sc::State state1, state2;
    state1.pos() = Vector3d(1, 0, 0);
    state1.quat().set(M_PI, 0, 0);

    state2.pos() = Vector3d(1, 0, 1);
    state2.quat().set(0, 0, 0);
    Vector3d position_local_to_state1 = \
        state1.rel_pos_local_frame(state2.pos());

    EXPECT_NEAR(0, position_local_to_state1(0), 1e-10);
    EXPECT_NEAR(0, position_local_to_state1(1), 1e-10);
    EXPECT_NEAR(-1, position_local_to_state1(2), 1e-10);

    Vector3d position_local_to_state2 = \
        state2.rel_pos_local_frame(state1.pos());

    EXPECT_NEAR(0, position_local_to_state2(0), 1e-10);
    EXPECT_NEAR(0, position_local_to_state2(1), 1e-10);
    EXPECT_NEAR(-1, position_local_to_state2(2), 1e-10);
}

TEST(test_state, rel_az) {
    sc::State state;
    state.pos() << 0, 0, 0;
    state.vel() << 0, 0, 0;
    state.quat().set(M_PI / 4, M_PI / 4, M_PI / 4);

    Eigen::Vector3d vec1(1, 0, 50);
    double rel_az1 = state.rel_az(vec1);

    Eigen::Vector3d vec2(0, 1, 50);
    double rel_az2 = state.rel_az(vec2);

    EXPECT_NEAR(-M_PI / 4, rel_az1, 1e-10);
    EXPECT_NEAR(M_PI / 4, rel_az2, 1e-10);
}
