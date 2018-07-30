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
#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include <scrimmage/proto/Scrimmage.grpc.pb.h>
#include <scrimmage/log/FrameUpdateClient.h>
#include <scrimmage/common/Utilities.h>

#include <scrimmage/math/Quaternion.h>

#include <GeographicLib/LocalCartesian.hpp>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using std::cout;
using std::endl;

namespace scrimmage {
    FrameUpdateClient::FrameUpdateClient(std::string ip, int port)
    {
        angles_to_gps_.set_input_clock_direction(Angles::Rotate::CCW);
        angles_to_gps_.set_input_zero_axis(Angles::HeadingZero::Pos_X);
        angles_to_gps_.set_output_clock_direction(Angles::Rotate::CW);
        angles_to_gps_.set_output_zero_axis(Angles::HeadingZero::Pos_Y);
    }

    bool FrameUpdateClient::send_frame(scrimmage_proto::Frame &frame)
    {

        // Change from local x, y, z to GPS coordinates:
        for (int i = 0; i < frame.contact_size(); i++) {
            //frame.contacts_[frame.contact(i).id().id()] = proto_2_contact(proto_frame.contact(i));
            double x = frame.contact(i).state().position().x();
            double y = frame.contact(i).state().position().y();
            double z = frame.contact(i).state().position().z();
            double lat, lon, h;
            proj_->Reverse(x, y, z, lat, lon, h);
            frame.mutable_contact(i)->mutable_state()->mutable_position()->set_x(lon);
            frame.mutable_contact(i)->mutable_state()->mutable_position()->set_y(lat);
            frame.mutable_contact(i)->mutable_state()->mutable_position()->set_z(h);

            x = frame.contact(i).state().orientation().x();
            y = frame.contact(i).state().orientation().y();
            z = frame.contact(i).state().orientation().z();
            double w = frame.contact(i).state().orientation().w();

            //scrimmage::Quaternion q(Eigen::Vector3d(x,y,z), w);
            scrimmage::Quaternion q(w,x,y,z);
            double yaw = q.yaw();

            angles_to_gps_.set_angle(Angles::rad2deg(yaw));
            yaw = Angles::deg2rad(angles_to_gps_.angle());
            q = scrimmage::Quaternion(q.roll(), q.pitch(), yaw);

            frame.mutable_contact(i)->mutable_state()->mutable_orientation()->set_x(q.x());
            frame.mutable_contact(i)->mutable_state()->mutable_orientation()->set_y(q.y());
            frame.mutable_contact(i)->mutable_state()->mutable_orientation()->set_z(q.z());
            frame.mutable_contact(i)->mutable_state()->mutable_orientation()->set_w(q.w());

            frame.mutable_contact(i)->set_type(scrimmage_proto::AIRCRAFT);
        }

        scrimmage_proto::BlankReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        grpc::ClientContext context;

        // The actual RPC.
        grpc::Status status;
        status = stub_->SendFrame(&context, frame, &reply);

        // Act upon its status.
        if (status.ok()) {
            return true;
        } else {
            cout << "Error code: " << status.error_code() << endl;
            cout << status.error_message() << endl;
            return false;
        }

        return true;
    }

    void FrameUpdateClient::set_projection(std::shared_ptr<GeographicLib::LocalCartesian> proj)
    {
        proj_ = proj;
    }
}
