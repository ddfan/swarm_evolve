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
#ifndef LOG_H_
#define LOG_H_
#include <list>
#include <fstream>
#include <map>
#include <scrimmage/entity/Contact.h>
#include <scrimmage/math/Quaternion.h>
#include <scrimmage/log/Frame.h>
#include <scrimmage/math/State.h>

#include <google/protobuf/message_lite.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include <scrimmage/proto/Frame.pb.h>
#include <scrimmage/proto/Shape.pb.h>

#if ENABLE_GRPC
#include <scrimmage/log/FrameUpdateClient.h>
#endif

#include <memory>

namespace GeographicLib { class LocalCartesian; }

namespace scrimmage {

class Log {
 public:
    using ZeroCopyInputStreamPtr = std::shared_ptr<google::protobuf::io::ZeroCopyInputStream>;
    using ZeroCopyOutputStreamPtr = std::shared_ptr<google::protobuf::io::ZeroCopyOutputStream>;

    Log();

    enum FileType {
        FRAMES = 0,
        SHAPES = 1,
        UTMTERRAIN = 2,
        SIMINFO = 3,
        CONTACTVISUAL = 4,
        GUIMSG = 5,
        MSG = 6
    };

    enum Mode {
        READ = 0,
        WRITE = 1,
        NONE
    };
    
    bool init(std::string dir, Mode mode);

    bool parse(std::string dir);
    bool parse(std::string filename, FileType type);

    bool parse_frames(std::string filename, 
                      ZeroCopyInputStreamPtr input);
    
    bool parse_shapes(std::string filename, 
                      ZeroCopyInputStreamPtr input);

    bool parse_utm_terrain(std::string filename, 
                           ZeroCopyInputStreamPtr input);

    bool parse_contact_visual(std::string filename, 
                              ZeroCopyInputStreamPtr input);
    
    /*
    bool create_frame(double time, ContactMapPtr &contacts,
                      std::list<sp::Event> &events,
                      std::shared_ptr<sp::Frame> &frame);
    */
    bool close_log();

    std::list<Frame > & scrimmage_frames();

    std::list<std::shared_ptr<scrimmage_proto::Frame> > & frames();
    std::list<std::shared_ptr<scrimmage_proto::Shapes> > & shapes();
    std::list<std::shared_ptr<scrimmage_proto::UTMTerrain> > & utm_terrain();
    std::list<std::shared_ptr<scrimmage_proto::ContactVisual> > & contact_visual();

    std::string log_dir();

    bool write_ascii(std::string str);

    bool save_frame(std::shared_ptr<scrimmage_proto::Frame> &frame);
    
    bool save_shapes(scrimmage_proto::Shapes &shapes);

    bool save_utm_terrain(std::shared_ptr<scrimmage_proto::UTMTerrain> &utm_terrain);

    bool save_contact_visual(std::shared_ptr<scrimmage_proto::ContactVisual> &contact_visual);

    //bool save_messages();
    
    std::string frames_filename();
    std::string shapes_filename();
    std::string utm_terrain_filename();
    std::string contact_visual_filename();
    std::string msgs_filename();

    void set_enable_log(bool enable);

    void init_network(NetworkPtr network);
    
 protected:
    using MessageLitePtr = std::shared_ptr<google::protobuf::MessageLite>;

    bool enable_log_;
    Mode mode_;

    bool open_file(std::string name, int &fd);

    std::string frames_name_;
    std::string shapes_name_;
    std::string utm_terrain_name_;
    std::string contact_visual_name_;
    std::string msgs_name_;

    int frames_fd_;
    int shapes_fd_;
    int utm_terrain_fd_;
    int contact_visual_fd_;
    int msgs_fd_;
    
    std::string log_dir_;

    std::string ascii_filename_;
    std::ofstream ascii_output_;    

    ZeroCopyOutputStreamPtr frames_output_;
    ZeroCopyOutputStreamPtr shapes_output_;
    ZeroCopyOutputStreamPtr utm_terrain_output_;
    ZeroCopyOutputStreamPtr contact_visual_output_;
    ZeroCopyOutputStreamPtr msgs_output_;

    std::list<Frame> scrimmage_frames_;
    std::list<std::shared_ptr<scrimmage_proto::Frame> > frames_;
    std::list<std::shared_ptr<scrimmage_proto::Shapes> > shapes_;
    std::list<std::shared_ptr<scrimmage_proto::UTMTerrain> > utm_terrain_;
    std::list<std::shared_ptr<scrimmage_proto::ContactVisual> > contact_visual_;

    PluginPtr pubsub_;
    SubscriberPtr sub_ent_collisions_;

    bool writeDelimitedTo(
        const google::protobuf::MessageLite& message,
        ZeroCopyOutputStreamPtr rawOutput);

    bool readDelimitedFrom(std::string filename,
                           ZeroCopyInputStreamPtr rawInput,
                           MessageLitePtr message,
                           bool& clean_eof);

private:
};
}
#endif
