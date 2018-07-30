/******************************************************************************
* Copyright 2016, Georgia Tech Research Corporation, Atlanta, Georgia 30332
* All Rights Reserved
******************************************************************************/
#ifndef QuadDefense_H_
#define QuadDefense_H_
#include <scrimmage/autonomy/Autonomy.h>
#include <scrimmage/pubsub/Publisher.h>
#include <scrimmage/pubsub/Subscriber.h>

class QuadDefense : public scrimmage::Autonomy{
public:
     QuadDefense();
     virtual void init(std::map<std::string,std::string> &params);
     virtual bool step_autonomy(double t, double dt);
protected:     
     double x_offset_;
     double y_offset_;
     double z_offset_;
             
     bool in_position_;
     double avoid_dist_;

     double fire_range_max_;
     double fire_FOV_;
     bool fire_2D_mode_;

     sc::PublisherPtr pub_fire_;
     sc::SubscriberPtr sub_oneteam_;
     
private:     
};

#endif
