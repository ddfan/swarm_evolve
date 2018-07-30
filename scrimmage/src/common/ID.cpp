#include <scrimmage/common/ID.h>

namespace scrimmage {

ID::ID() : id_(0), sub_swarm_id_(0), team_id_(0) {}

ID::ID(int id, int sub_swarm_id, int team_id) :
    id_(id), sub_swarm_id_(sub_swarm_id), team_id_(team_id) {}

void ID::set_id(int id) { id_ = id; }

void ID::set_sub_swarm_id(int sub_swarm_id) {sub_swarm_id_ = sub_swarm_id; }

void ID::set_team_id(int team_id) { team_id_ = team_id; }

int ID::id() { return id_; }

int ID::sub_swarm_id() { return sub_swarm_id_; }

int ID::team_id() { return team_id_; }

bool ID::operator==(const ID &other) const {
    return id_ == other.id_ &&
            sub_swarm_id_ == other.sub_swarm_id_ &&
            team_id_ == other.team_id_;
}


}
