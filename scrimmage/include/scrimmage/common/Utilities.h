/*
 * ---------------------------------------------------------------------------
 * @section LICENSE
 *  
 * Copyright (c) 2016 Georgia Tech Research Institute (GTRI) 
 *               All Rights Reserved
 *  
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 * ---------------------------------------------------------------------------
 * @file filename.ext
 * @author Kevin DeMarco <kevin.demarco@gtri.gatech.edu> 
 * @author Eric Squires <eric.squires@gtri.gatech.edu>
 * @version 1.0
 * ---------------------------------------------------------------------------
 * @brief A brief description.
 * 
 * @section DESCRIPTION
 * A long description.
 * ---------------------------------------------------------------------------
 */
#ifndef SC_UTILITIES_H_
#define SC_UTILITIES_H_

#include <map>
#include <vector>
#include <Eigen/Dense>


namespace scrimmage {

void display_progress(float progress);

int next_available_id(std::string name,
                      std::map<std::string, std::string> &info,
                      std::map<int,int> &id_map);

std::string get_sha(std::string &path);

std::string get_version();

void filter_line(int downsampling_factor,
    int num_points,
    std::vector<Eigen::Vector3d> &path,
    std::vector<Eigen::Vector3d> &filtered_path);

std::string generate_chars(std::string symbol, int num);

template <class T>
class AdamOptimizer {
private:
    double beta1=0.9;
    double beta2=0.999;
    double epsilon=1.0e-8;
    T m;
    T v;

public:
    void setparams(double beta1_, double beta2_, double epsilon_){
        beta1=beta1_;
        beta2=beta2_;
        epsilon=epsilon_;
    }

    T step(T grad){
        //first time running, set to zeros
        if (grad.size() != m.size()){
            m.assign(grad.size(),0.0);
            v.assign(grad.size(),0.0);
        }

        T update(m.size(),0.0);
        for(size_t i=0;i<m.size();i++){
            m[i]=beta1*m[i]+(1.0-beta1)*grad[i];
            v[i]=beta2*v[i]+(1.0-beta2)*grad[i]*grad[i];
            update[i]=m[i]/(1.0-beta1)/(std::sqrt(v[i]/(1.0-beta2))+epsilon);
        }

        return update;
    }
};

}

#endif
