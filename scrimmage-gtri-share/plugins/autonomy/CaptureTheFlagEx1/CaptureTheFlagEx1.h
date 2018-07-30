/******************************************************************************
* Copyright 2016, Georgia Tech Research Corporation, Atlanta, Georgia 30332
* All Rights Reserved
******************************************************************************/
#ifndef CaptureTheFlagEx1_H_
#define CaptureTheFlagEx1_H_
#include <memory>
#include <scrimmage/autonomy/Autonomy.h>

class CaptureTheFlagEx1 : public scrimmage::Autonomy {
public:

     typedef enum Mode {
          GreedyShooter = 0,
          DiveBomber,
          Defender
     } Mode_t;
     
     CaptureTheFlagEx1();
     ~CaptureTheFlagEx1() {}
     virtual void init(std::map<std::string,std::string> &params);
     virtual bool step_autonomy(double t, double dt);
     virtual bool posthumous(double t);
protected:
     Mode_t mode_;
     double dist_xy_;
     double dive_altitude_;
     double avoid_dist_;
     double fire_range_max_;
     double fire_FOV_;
     bool fire_2D_mode_;

     scrimmage::PublisherPtr pub_fire_;
private:     
};

#endif
