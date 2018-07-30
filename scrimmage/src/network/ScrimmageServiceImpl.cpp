#if ENABLE_GRPC

#include <scrimmage/network/ScrimmageServiceImpl.h>

scrimmage::ScrimmageServiceImpl::ScrimmageServiceImpl(scrimmage::Interface *interface) : interface_(interface){ }

grpc::Status scrimmage::ScrimmageServiceImpl::SendFrame(grpc::ServerContext *context, const scrimmage_proto::Frame *frame, scrimmage_proto::BlankReply *reply) {
    std::shared_ptr<scrimmage_proto::Frame> f = std::make_shared<scrimmage_proto::Frame>(*frame);
    interface_->push_frame(f);
    return grpc::Status::OK;
}

grpc::Status scrimmage::ScrimmageServiceImpl::SendUTMTerrain(grpc::ServerContext *context, const scrimmage_proto::UTMTerrain *terrain, scrimmage_proto::BlankReply *reply) {
    std::shared_ptr<scrimmage_proto::UTMTerrain> t = std::make_shared<scrimmage_proto::UTMTerrain>(*terrain);
    interface_->push_utm_terrain(t);
    return grpc::Status::OK;
}

grpc::Status scrimmage::ScrimmageServiceImpl::SendSimInfo(grpc::ServerContext *context, const scrimmage_proto::SimInfo *sim_info, scrimmage_proto::BlankReply *reply) {
    scrimmage_proto::SimInfo si = *sim_info;
    interface_->push_sim_info(si);
    return grpc::Status::OK;
}

grpc::Status scrimmage::ScrimmageServiceImpl::SendGUIMsg(grpc::ServerContext *context, const scrimmage_proto::GUIMsg *gui_msg, scrimmage_proto::BlankReply *reply) {
    scrimmage_proto::GUIMsg si = *gui_msg;
    interface_->push_gui_msg(si);
    return grpc::Status::OK;
}

grpc::Status scrimmage::ScrimmageServiceImpl::SendContactVisual(grpc::ServerContext *context, const scrimmage_proto::ContactVisual *contact_visual, scrimmage_proto::BlankReply *reply) {
    std::shared_ptr<scrimmage_proto::ContactVisual> cv = std::make_shared<scrimmage_proto::ContactVisual>(*contact_visual);
    interface_->push_contact_visual(cv);
    return grpc::Status::OK;
}

grpc::Status scrimmage::ScrimmageServiceImpl::SendShapes(grpc::ServerContext *context, const scrimmage_proto::Shapes *shape, scrimmage_proto::BlankReply *reply) {
    scrimmage_proto::Shapes s = *shape;
    interface_->push_shapes(s);
    return grpc::Status::OK;
}

#endif // ENABLE_GRPC
