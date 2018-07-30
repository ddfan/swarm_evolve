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
#ifndef MISSIONPARSE_H_
#define MISSIONPARSE_H_
#include <list>
#include <vector>
#include <map>
#include <memory>

#include <scrimmage/fwd_decl.h>
#include <scrimmage/proto/Visual.pb.h>

#include <Eigen/Dense>
#include <scrimmage/proto/Color.pb.h>

namespace sp = scrimmage_proto;

namespace scrimmage {

// Key 1: Entity Description XML ID
// Value 1: Map of entity information
// Key 2: entity param string
// Value 2: string value for entity param
typedef std::map<int, std::map<std::string, std::string>> EntityDesc_t;

// key1 = xml node name
// key2 = xml node attribute name
// value = xml node attribute value
typedef std::map<std::string, std::map<std::string, std::string>> AttributeMap;

struct TeamInfo {
    int team_id;
    scrimmage_proto::Color color;
    std::list<Eigen::Vector3d> bases;
    std::vector<double> radii;
};

struct GenerateInfo {
    double start_time;
    int total_count;        
    int gen_count;
    double rate;  
    bool first_in_group;        
    double time_variance;
};

class MissionParse {
public:
    MissionParse();
    void set_log_dir(std::string);
    bool create_log_dir();
    bool create_log_dir(bool);
    bool parse(std::string filename);
    bool write(std::string filename);

    double t0();
    double tend();
    double dt();
    double motion_multiplier();
    double time_warp();
    bool start_paused();

    bool parse_terrain();

    scrimmage_proto::Color & background_color();


    std::string log_dir();

    std::map<int, AttributeMap> &entity_attributes();
    EntityDesc_t & entity_descriptions();

    bool enable_gui();
    bool network_gui();

    AttributeMap &attributes();
    std::map<std::string,std::string> & params();

    double longitude_origin();
    double latitude_origin();
    double altitude_origin();

    std::map<int,TeamInfo> & team_info();

    void set_task_number(int task_num);
    void set_job_number(int job_num);

    std::list<std::string> entity_interactions();

    std::list<std::string> metrics();

    std::map<int, GenerateInfo > & gen_info();

    std::map<int, std::vector<double> > & next_gen_times();

    std::shared_ptr<GeographicLib::LocalCartesian> projection();
    
    std::shared_ptr<scrimmage_proto::UTMTerrain> & utm_terrain();
    
protected:
    std::string mission_filename_;

    double t0_;
    double tend_;
    double dt_;
    int motion_multiplier_;
    double time_warp_;

    bool enable_gui_;
    bool network_gui_;
    bool start_paused_;

    AttributeMap attributes_;
    std::map<std::string,std::string> params_;

    std::map<int, TeamInfo > team_info_;

    std::map<int, AttributeMap> entity_attributes_;
    EntityDesc_t entity_descs_;

    std::string log_dir_;

    double longitude_origin_;
    double latitude_origin_;
    double altitude_origin_;

    scrimmage_proto::Color background_color_;
        
    int task_number_;
    int job_number_;

    std::list<std::string> entity_interactions_;
    std::list<std::string> metrics_;

    // Key: entity_description ID in EntityDesc_t map
    // Value: generation
    std::map<int, GenerateInfo> gen_info_;

    // Key: entity_description ID in EntityDesc_t vector
    std::map<int, std::vector<double> > next_gen_times_;

    std::shared_ptr<GeographicLib::LocalCartesian> proj_;            

    std::shared_ptr<scrimmage_proto::UTMTerrain> utm_terrain_;
    
private:
};
using MissionParsePtr = std::shared_ptr<MissionParse>;
}

#endif
