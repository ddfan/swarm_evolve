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
#ifndef _SCRIMMAGE_COLORMAPS_
#define _SCRIMMAGE_COLORMAPS_

// Plugins use the Colormap functions, but this compilation unit doesn't know
// that. Disable the unused function warning.

namespace scrimmage {

typedef struct Color{
    double r,g,b;
} Color_t;

// Matlab's versions of JET to GRAYSCALE
// Matlab uses the same JET version as BlueView sonar JET
Color_t GetColor_matlab(double v, double vmin, double vmax);
double GetGray_matlab(Color_t c, double vmin, double vmax);

// "Traditional" JET conversions
Color_t GetColor(double v, double vmin, double vmax);

int GetGray(Color_t c, double vmin, double vmax);

}

#endif
