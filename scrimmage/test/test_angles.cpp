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
    sc::Angles ang;
    ang.set_input_clock_direction(sc::Angles::Rotate::CW);
    ang.set_input_zero_axis(sc::Angles::HeadingZero::Pos_Y);
    ang.set_output_clock_direction(sc::Angles::Rotate::CCW);
    ang.set_output_zero_axis(sc::Angles::HeadingZero::Pos_X);

    ang.set_angle(134);
    EXPECT_NEAR(ang.angle(), 360-44, 1e-1);
}
