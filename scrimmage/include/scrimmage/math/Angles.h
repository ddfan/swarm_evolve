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
#ifndef ANGLES_H_
#define ANGLES_H_

namespace scrimmage {
class Angles {
 public:
    Angles();

    enum class Rotate {
        CCW = 0,
        CW
    };

///           Pos_Y
///             |
///             |
///   Neg_X --- 0 --- Pos_X
///             |
///             |
///           Neg_Y

    enum class HeadingZero {
        Pos_X = 1,
        Pos_Y = 2,
        Neg_X = 3,
        Neg_Y = 4
    };

    // Degrees to/from radians conversion
    static double deg2rad(double deg);
    static double rad2deg(double rad);

    // Seup the input coordinate system's clock direction and zero axis
    void set_input_clock_direction(Rotate direction);
    
    void set_input_zero_axis(HeadingZero zero);

    // Setup the input coordinate system's clock direction and zero axis
    void set_output_clock_direction(Rotate direction);
    
    void set_output_zero_axis(HeadingZero zero);

    // Is the provided angle between 0 and 359 degrees, inclusive?
    bool is_angle_360(double angle);

    // Normalize angle to 0-359 degrees
    static double angle_360(double angle);
    
    static double angle_2pi(double angle);
    
    // Normalize angle to 0-179 degrees
    static double angle_180(double angle);

    static double angle_pi(double angle);

    // Calculate shortest distance between two angles
    static double angle_diff(double ang1, double ang2);
    
    static double angle_diff_rad(double ang1, double ang2);

    static double angle_avg(double ang1, double ang2);
    
    static double angle_avg_rad(double ang1, double ang2);

    // Set the input coordinate system's angle that the user wants to convert
    void set_angle(double angle);

    // Get the output coordinate system's computed angle.
    double angle();

 protected:
    double angle_;

    Rotate in_direction_;
    HeadingZero in_zero_;

    Rotate out_direction_;
    HeadingZero out_zero_;

 private:
};
} // namespace scrimmage
#endif
