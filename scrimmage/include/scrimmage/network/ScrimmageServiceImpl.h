#ifndef _SCRIMMAGE_IMPL_H_
#define _SCRIMMAGE_IMPL_H_
/// ---------------------------------------------------------------------------
/// @file ScrimmageServiceImpl.h
/// @author Kevin DeMarco <kevin.demarco@gmail.com>
///
/// Time-stamp: <2016-10-12 11:33:48 syllogismrxs>
///
/// @version 1.0
/// Created: 27 Feb 2017
///
/// ---------------------------------------------------------------------------
/// @section LICENSE
///
/// The MIT License (MIT)
/// Copyright (c) 2012 Kevin DeMarco
///
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following conditions:
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
/// @section DESCRIPTION
///
/// The Interface class ...
///
/// ---------------------------------------------------------------------------
#include <list>
#include <mutex>

#include <scrimmage/proto/Frame.pb.h>
#include <scrimmage/proto/Visual.pb.h>
#include <scrimmage/proto/Scrimmage.grpc.pb.h>
#include <scrimmage/network/Interface.h>

using std::cout;
using std::endl;

namespace scrimmage {

    class ScrimmageServiceImpl final : public scrimmage_proto::ScrimmageService::Service {
    public:
        explicit ScrimmageServiceImpl(Interface *interface);

        grpc::Status SendFrame(grpc::ServerContext* context, const scrimmage_proto::Frame* frame,
                               scrimmage_proto::BlankReply* reply) override;

        grpc::Status SendUTMTerrain(grpc::ServerContext* context, const scrimmage_proto::UTMTerrain* terrain,
                                    scrimmage_proto::BlankReply* reply) override;

        grpc::Status SendSimInfo(grpc::ServerContext* context, const scrimmage_proto::SimInfo* sim_info,
                                    scrimmage_proto::BlankReply* reply) override;

        grpc::Status SendGUIMsg(grpc::ServerContext* context, const scrimmage_proto::GUIMsg* gui_msg,
                                    scrimmage_proto::BlankReply* reply) override;

        grpc::Status SendContactVisual(grpc::ServerContext* context,
                                       const scrimmage_proto::ContactVisual* contact_visual,
                                       scrimmage_proto::BlankReply* reply) override;

        grpc::Status SendShapes(grpc::ServerContext* context,
                                       const scrimmage_proto::Shapes* shape,
                                       scrimmage_proto::BlankReply* reply) override;
    protected:
        Interface * interface_;
    private:
    };
}
#endif
