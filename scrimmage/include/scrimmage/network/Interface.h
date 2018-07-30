#ifndef SCRIMMAGE_INTERFACE_H_
#define SCRIMMAGE_INTERFACE_H_
/// ---------------------------------------------------------------------------
/// @file Interface.h
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

#include <scrimmage/fwd_decl.h>
#include <scrimmage/proto/Frame.pb.h>
#include <scrimmage/proto/GUIControl.pb.h>

#if ENABLE_GRPC
#include <scrimmage/proto/Scrimmage.grpc.pb.h>
#endif


namespace sp = scrimmage_proto;
namespace sc = scrimmage;

namespace scrimmage {
    class Interface {
    public:

        typedef enum Mode {
            shared = 0,
            client = 1,
            server = 2
        } Mode_t;

        Interface();

        void set_mode(Mode_t mode) { mode_ = mode; }
        void set_ip(std::string &ip) { ip_ = ip; }
        void set_port(int port) { port_ = port; }
        bool init_network(Mode_t mode, std::string ip, int port);

        bool frames_update()
        {
            frames_mutex.lock();
            bool status = frames_list_.size() > 0;
            frames_mutex.unlock();
            return status;
        }

        bool utm_terrain_update()
        {
            utm_terrain_mutex.lock();
            bool status = utm_terrain_list_.size() > 0;
            utm_terrain_mutex.unlock();
            return status;
        }

        bool contact_visual_update()
        {
            contact_visual_mutex.lock();
            bool status = contact_visual_list_.size() > 0;
            contact_visual_mutex.unlock();
            return status;
        }

        bool gui_msg_update()
        {
            gui_msg_mutex.lock();
            bool status = gui_msg_list_.size() > 0;
            gui_msg_mutex.unlock();
            return status;
        }

        bool sim_info_update()
        {
            sim_info_mutex.lock();
            bool status = sim_info_list_.size() > 0;
            sim_info_mutex.unlock();
            return status;
        }

        bool shapes_update()
        {
            shapes_mutex.lock();
            bool status = shapes_list_.size() > 0;
            shapes_mutex.unlock();
            return status;
        }

        // SimControl to GUI
        bool send_frame(double time,
                        ContactMapPtr &contacts);

        bool send_frame(std::shared_ptr<scrimmage_proto::Frame> &frame);

        bool send_utm_terrain(std::shared_ptr<scrimmage_proto::UTMTerrain> &utm_terrain);
        bool send_contact_visual(std::shared_ptr<scrimmage_proto::ContactVisual> &cv);

        bool push_frame(std::shared_ptr<scrimmage_proto::Frame> &frame);
        bool push_utm_terrain(std::shared_ptr<scrimmage_proto::UTMTerrain> &utm_terrain);
        bool push_contact_visual(std::shared_ptr<scrimmage_proto::ContactVisual> &cv);

        // GUI to SimControl
        bool send_gui_msg(scrimmage_proto::GUIMsg &gui_msg);
        bool push_gui_msg(scrimmage_proto::GUIMsg &gui_msg);

        bool send_sim_info(scrimmage_proto::SimInfo &sim_info);
        bool push_sim_info(scrimmage_proto::SimInfo &sim_info);

        bool send_shapes(scrimmage_proto::Shapes &shapes);
        bool push_shapes(scrimmage_proto::Shapes &shapes);

        std::mutex frames_mutex;
        std::mutex utm_terrain_mutex;
        std::mutex gui_msg_mutex;
        std::mutex sim_info_mutex;
        std::mutex shapes_mutex;
        std::mutex contact_visual_mutex;

        std::list<std::shared_ptr<scrimmage_proto::Frame> > & frames()
        { return frames_list_; }

        std::list<std::shared_ptr<scrimmage_proto::UTMTerrain> > & utm_terrain()
        { return utm_terrain_list_; }

        std::list<std::shared_ptr<scrimmage_proto::ContactVisual> > & contact_visual()
        { return contact_visual_list_; }

        std::list<scrimmage_proto::GUIMsg> & gui_msg()
        { return gui_msg_list_; }

        std::list<scrimmage_proto::SimInfo> & sim_info()
        { return sim_info_list_; }

        std::list<scrimmage_proto::Shapes> & shapes()
        { return shapes_list_; }

        void set_log(std::shared_ptr<sc::Log> log) { log_ = log; }

        void send_cached();

    protected:

        Mode_t mode_;
        int port_;
        std::string ip_;

        std::list<std::shared_ptr<scrimmage_proto::Frame> > frames_list_;
        std::list<std::shared_ptr<scrimmage_proto::UTMTerrain> > utm_terrain_list_;
        std::list<std::shared_ptr<scrimmage_proto::ContactVisual> > contact_visual_list_;

        std::list<scrimmage_proto::GUIMsg> gui_msg_list_;
        std::list<scrimmage_proto::SimInfo> sim_info_list_;
        std::list<scrimmage_proto::Shapes> shapes_list_;

        unsigned int max_queue_size_;

#if ENABLE_GRPC
        std::unique_ptr<scrimmage_proto::ScrimmageService::Stub> scrimmage_stub_;
#endif

        // Connection timeout in seconds
        unsigned int client_timeout_;

        std::shared_ptr<sc::Log> log_;

        // Cached variables that the gui can request:
        bool caching_enabled_;
        std::shared_ptr<scrimmage_proto::UTMTerrain> utm_terrain_cache_;
        std::list<std::shared_ptr<scrimmage_proto::ContactVisual> > contact_visual_cache_;

    private:
    };
    using InterfacePtr = std::shared_ptr<Interface>;
}
#endif
