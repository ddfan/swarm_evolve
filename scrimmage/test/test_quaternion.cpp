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
#include <scrimmage/math/Quaternion.h>
#include <scrimmage/math/Angles.h>
#include <Eigen/Dense>
#define _USE_MATH_DEFINES
#include <cmath>

using Eigen::Vector3d;
namespace sc = scrimmage;

TEST(test_quaternion, rotation) {
    Vector3d vector = Vector3d(1, 0, 0);
    sc::Quaternion quaternion(Vector3d(1, 1, 1), 2 * M_PI / 3);
    Vector3d rotated_vector = quaternion.rotate(vector);
    EXPECT_NEAR(rotated_vector(0), 0, 1e-10);
    EXPECT_NEAR(rotated_vector(1), 1, 1e-10);
    EXPECT_NEAR(rotated_vector(2), 0, 1e-10);
}

TEST(test_quaternion, reverse_rotation) {
    Vector3d vector = Vector3d(1, 0, 0);
    sc::Quaternion quaternion(Vector3d(1, 1, 1), -2 * M_PI / 3);
    Vector3d rotated_vector = quaternion.rotate_reverse(vector);
    EXPECT_NEAR(rotated_vector(0), 0, 1e-10);
    EXPECT_NEAR(rotated_vector(1), 1, 1e-10);
    EXPECT_NEAR(rotated_vector(2), 0, 1e-10);
}

TEST(test_quaternion, euler_convert) {
    double roll = 0.3;
    double pitch = 0.2;
    double yaw = 0.1;
    sc::Quaternion quaternion(roll, pitch, yaw);
    EXPECT_NEAR(roll, quaternion.roll(), 1e-10);
    EXPECT_NEAR(pitch, quaternion.pitch(), 1e-10);
    EXPECT_NEAR(yaw, quaternion.yaw(), 1e-10);
}

TEST(test_quaternion, frames) {
    Vector3d vec1(0, 0, 0);
    Vector3d vec2(1, 0, 1);
    Vector3d vec_diff = vec2 - vec1;

    // pointing upward with negative pitch
    sc::Quaternion q1(0, -sc::Angles::deg2rad(45), 0);  

    Vector3d vec_diff_local1 = q1.rotate_reverse(vec_diff);
    EXPECT_NEAR(sqrt(2), vec_diff_local1(0), 1e-10);
    EXPECT_NEAR(0, vec_diff_local1(1), 1e-10);
    EXPECT_NEAR(0, vec_diff_local1(2), 1e-10);

    Vector3d vec_diff_global = q1.rotate(vec_diff_local1);
    EXPECT_NEAR(vec_diff(0), vec_diff_global(0), 1e-10);
    EXPECT_NEAR(vec_diff(1), vec_diff_global(1), 1e-10);
    EXPECT_NEAR(vec_diff(2), vec_diff_global(2), 1e-10);
}
