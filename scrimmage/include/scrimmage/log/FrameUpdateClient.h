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
#ifndef FRAMEUPDATECLIENT_H_
#define FRAMEUPDATECLIENT_H_
#include <iostream>
#include <memory>
#include <string>

#include <scrimmage/proto/Scrimmage.grpc.pb.h>

#include <scrimmage/fwd_decl.h>
#include <memory>

#include <scrimmage/math/Angles.h>

namespace scrimmage {
class FrameUpdateClient {
public:
    FrameUpdateClient(std::string ip, int port);
    bool send_frame(scrimmage_proto::Frame &frame);

    void set_projection(std::shared_ptr<GeographicLib::LocalCartesian> proj);
private:
    std::unique_ptr<scrimmage_proto::ScrimmageService::Stub> stub_;
    std::shared_ptr<GeographicLib::LocalCartesian> proj_;
    Angles angles_to_gps_;
};
}

#endif
