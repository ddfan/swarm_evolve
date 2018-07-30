#include <scrimmage/common/Random.h>
#include <chrono>

namespace scrimmage {

Random::Random() : rng_normal_(0, 1), rng_uniform_(-1, 1) {}

uint32_t Random::get_seed() {return seed_;}

void Random::seed() {
    seed(std::chrono::system_clock::now().time_since_epoch().count());
}

void Random::seed(uint32_t _seed) {
    seed_ = _seed;
    gener_.seed(seed_);
}

double Random::rng_uniform() {return rng_uniform_(gener_);}

double Random::rng_normal() {return rng_normal_(gener_);}

int Random::rng_uniform_int(int low, int high) {
    return std::uniform_int_distribution<int>(low, high)(gener_);
}

int Random::rng_discrete_int(std::vector<double> &weights)
{
    std::discrete_distribution<int> dist (weights.begin(), weights.end());
    return dist(gener_);
}

std::default_random_engine &Random::gener() {return gener_;}

}
