#include "control.h"

void PxController::init(float v_init_x, float v_init_y,
                    Vector2f origin, Vector2f pos) {
    v_ << v_init_x, v_init_y;
    drift_ = pos - origin;
    setStartPoint(pos);
}

void PxController::addTime(double dt) {
    flight_time_ += dt;
}

void PxController::bound(Vector2f &n) {
    n.x() = -n.x();
    if(n.dot(v_) > 0) {
        return;
    }
    v_ += -2*(n.dot(v_))*n;
}

void PxController::bound(Vector2f &n, Vector2f &n2) {
    n += n2;
    n.normalize();
    bound(n);
}

void PxController::boundHandler(int boundary_cnt, 
        Vector2f norm1, Vector2f norm2 ,Vector2f pos) {
    if (boundary_cnt == 0) {
        return;
    }else{
        setStartPoint(pos);
        if (boundary_cnt == 1) {
            bound(norm1);
        }else{
            bound(norm1,norm2);
        }
    }
}


void PxController::setStartPoint(Vector2f pos) {
    flight_time_ = 0;
    start_point_ = pos;
}

Vector2f PxController::controlStep(double dt) {
    input_ = start_point_ + flight_time_*v_;
    addTime(dt);
    return input_;
}
