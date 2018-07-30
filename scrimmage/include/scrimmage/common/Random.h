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
#ifndef RANDOM_H_
#define RANDOM_H_
#include <memory>
#include <random>


namespace scrimmage {

class Random {
 public:
    Random();

    uint32_t get_seed();
    void seed();

    void seed(uint32_t _seed);

    double rng_uniform();
    double rng_normal();
    int rng_uniform_int(int low, int high);

    int rng_discrete_int(std::vector<double> & weights);

    std::default_random_engine &gener();

 protected:
    uint32_t seed_;
    std::default_random_engine gener_;
    std::normal_distribution<double> rng_normal_;
    std::uniform_real_distribution<double> rng_uniform_;
    //std::discrete_distribution<long> rng_discrete_;

private:
};

typedef std::shared_ptr<Random> RandomPtr;
}

#endif
