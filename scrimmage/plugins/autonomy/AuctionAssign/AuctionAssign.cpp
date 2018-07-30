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
#include <iostream>

#include <limits>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/common/Random.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/math/State.h>
#include <memory>

#include "AuctionAssign.h"

#include <scrimmage/pubsub/Network.h>
#include <scrimmage/pubsub/Publisher.h>
#include <scrimmage/pubsub/Subscriber.h>
#include <scrimmage/pubsub/Message.h>

#include <scrimmage/msgs/AuctionMsgs.pb.h>

namespace sc = scrimmage;

using BidMsg = sc::Message<auction::BidAuction>;

REGISTER_PLUGIN(scrimmage::Autonomy, AuctionAssign, AuctionAssign_plugin)

AuctionAssign::AuctionAssign() {}

void AuctionAssign::init(std::map<std::string,std::string> &params)
{
    id_ = parent_.lock()->id().id();

    desired_state_->vel() << 0,0,0;
    desired_state_->quat().set(0,0,state_->quat().yaw());
    desired_state_->pos() = Eigen::Vector3d::UnitZ()*state_->pos()(2);

    create_publisher("StartAuction");
    create_publisher("BidAuction");

    create_subscriber("StartAuction");
    create_subscriber("BidAuction");

    auction_started_ = false;
    auction_in_prog_ = false;
    auction_max_time_ = 5;
    max_bid_ = -std::numeric_limits<double>::max();
}

bool AuctionAssign::step_autonomy(double t, double dt)
{
    

    // Read the Start Auction inbox
    for (auto msg : subs_["StartAuction"]->pop_msgs()) {
        std::cout << "StartAuction: " << id_
          << " received message from " << msg->sender << std::endl;

        auto msg_bid = std::make_shared<BidMsg>();
        msg_bid->sender = id_;
        msg_bid->data.set_bid(parent_.lock()->random()->rng_uniform() * 10.0);
        msg_bid->data.SerializeToString(&msg_bid->serialized_data);
#if ENABLE_PYTHON_BINDINGS==1 
        msg_bid->serialize_to_python("AuctionMsgs_pb2", "BidAuction");
#endif 
        pubs_["BidAuction"]->publish(msg_bid);
    }

    if (auction_started_) {
        // Read the Bid Auction inbox
        for (auto msg : subs_["BidAuction"]->pop_msgs<BidMsg>()) {

            std::cout << "BidAuction: " << id_ << " received message from "
                << msg->sender << " bid: " << msg->data.bid() << std::endl;

            if (msg->data.bid() > max_bid_) {
                max_bid_ = msg->data.bid();
                max_bid_champ_ = msg->sender;
            }
        }
    }

    if (!auction_started_ && id_ == 1) {
        sc::MessageBasePtr msg(new sc::MessageBase());
        msg->sender = id_;

        std::cout << "origin: " << msg->sender << std::endl;
        pubs_["StartAuction"]->publish(msg);

        auction_started_ = true;
        auction_in_prog_ = true;
        auction_start_time_ = t;
        std::cout << "Starting!" << std::endl;
    }

    if (auction_in_prog_ && t > auction_start_time_ + auction_max_time_
        && id_ == 1) {
        std::cout << "======================================" << std::endl;
        std::cout << "Auction Complete" << std::endl;
        std::cout << "Max Bidder: " << max_bid_champ_ << " - Bid=" << max_bid_ << std::endl;
        std::cout << "======================================" << std::endl;
        auction_in_prog_ = false;
    }

    return true;
}
