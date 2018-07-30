/******************************************************************************
 * Copyright 2016, Georgia Tech Research Corporation, Atlanta, Georgia 30332
 * All Rights Reserved
 ******************************************************************************/
#ifndef CAPTURE_THE_FLAG_INTERACTION_H_
#define CAPTURE_THE_FLAG_INTERACTION_H_

#include <scrimmage/simcontrol/EntityInteraction.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/pubsub/Publisher.h>
#include <scrimmage/pubsub/Subscriber.h>

namespace sc = scrimmage;

class CaptureTheFlagInteraction : public scrimmage::EntityInteraction {
public:

    enum class HitDetection {
        cone = 0,
        noisy_cone = 1,
        shrinking_cone = 2,
        acs = 3
    };
    
    CaptureTheFlagInteraction();
    virtual bool init(std::map<std::string,std::string> &mission_params,
                      std::map<std::string,std::string> &plugin_params);
    
    virtual bool step_entity_interaction(std::list<sc::EntityPtr> &ents, 
                                         double t, double dt);
    
    virtual bool collision_exists(std::list<sc::EntityPtr> &ents,
                                  Eigen::Vector3d &p);
protected:
    
    bool create_cube(std::string cube_name, 
                     std::map<std::string,std::string> &plugin_params,
                     Eigen::Vector3d color);
    
    double collision_range_;
    double base_collision_range_;        

    bool enable_cube_effects_;
    double fire_range_max_;
    double offensive_fire_range_max_;
    double defensive_fire_range_max_;
    double fire_fov_width_;
    double fire_fov_height_;

    double fire_width_noise_;
    double fire_height_noise_;
    double fire_effective_range_;

    double fire_period_min_;
    int fire_rounds_max_;
    
    double entity_radius_;

    HitDetection hit_detection_;

    bool show_cubes_;

    class CubeInfo {
    public:       
        std::vector<Eigen::Vector3d> points;
        Eigen::Vector3d u;
        Eigen::Vector3d v;
        Eigen::Vector3d w;
        double u_dot_P0;
        double u_dot_P1;
        double v_dot_P0;
        double v_dot_P3;
        double w_dot_P0;
        double w_dot_P4;

        void compute_dots()
        {
            // Source: http://math.stackexchange.com/questions/1472049/check-if-a-point-is-inside-a-rectangular-shaped-area-3d
            u = points[0] - points[1];
            v = points[0] - points[3];
            w = points[0] - points[4];
            
            u_dot_P0 = u.dot(points[0]);
            u_dot_P1 = u.dot(points[1]);
            v_dot_P0 = v.dot(points[0]);
            v_dot_P3 = v.dot(points[3]);
            w_dot_P0 = w.dot(points[0]);
            w_dot_P4 = w.dot(points[4]);
        }
        
        bool contains(Eigen::Vector3d p)
        {
            double u_dot_p = u.dot(p);
            double v_dot_p = v.dot(p);
            double w_dot_p = w.dot(p);

            if ( (u_dot_P0 > u_dot_p) && (u_dot_p > u_dot_P1) &&
                 (v_dot_P0 > v_dot_p) && (v_dot_p > v_dot_P3) &&
                 (w_dot_P0 > w_dot_p) && (w_dot_p > w_dot_P4)) {
                return true;
            }
            return false;
        }
    };

    std::map<std::string, CubeInfo> cubes_;

    // Key: ID of entity
    // Value: Last time the entity fired its weapon
    std::map<int,double> prev_fire_;

    // Key: ID of entity
    // Value: Number of fire events from this ID
    std::map<int,int> num_fires_;

    sc::SubscriberPtr sub_fire_;
    sc::SubscriberPtr sub_ent_pres_end_;
    sc::PublisherPtr pub_base_coll_;        
    sc::PublisherPtr pub_fire_result_;
    sc::PublisherPtr pub_distancefrombase_;
    
private:     
};

#endif
