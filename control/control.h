#ifndef CONTROL_H_
#define CONTROL_H_

#include <Eigen/Core>

using namespace Eigen;

class PxController {

    Vector2f v_;
    Vector2f start_point_;
    Vector2f input_;
    Vector2f drift_;

    double flight_time_;

public:

    PxController(){
    }
    PxController(const PxController &o) : v_(o.v_) {
    }
    ~PxController(){}

    void init(float v_init_x, float v_init_y,
        Vector2f origin, Vector2f pos);

    float vx() {
        return v_.x();
    }
    float vy() {
        return v_.y();
    }

    void addTime(double dt);

    void setStartPoint(Vector2f pos);

    void bound(Vector2f &n);

    void bound(Vector2f &n, Vector2f &n2);

    void boundHandler(int boundary_cnt, Vector2f norm1, 
                            Vector2f norm2, Vector2f pos);

    Vector2f controlStep(double dt);
};

#endif
