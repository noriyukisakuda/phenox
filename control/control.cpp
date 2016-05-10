#include "control.h"
#include <iostream>
#define BOUNDLOCKTIME 2
#define BOOSTTIME 5
#define BOOSTPARAM 2


void PxController::init(float v_init_x, float v_init_y,
                    Vector2f origin, Vector2f pos) {
    v_ << v_init_x, v_init_y;
    drift_ = pos - origin;
    setStartPoint(pos);
    bound_locked_ = false;
    boost_count_ = 0;
}

void PxController::addTime(double dt) {
    flight_time_ += dt;
}

int PxController::bound(Vector2f &n) {
    std::cout << "----bound----- "<< std::endl;
    n.x() = -n.x();
    std::cout << " n:" << n.x() << "," << n.y() << std::endl;
    if(n.dot(v_) >= 0) {
        return 1;
    }
    v_ += -2*(n.dot(v_))*n;
    bound_locked_ = true;
    std::cout << " v:" << v_.x() << "," << v_.y() << std::endl;
    return 0;
}

int PxController::bound(Vector2f &n, Vector2f &n2) {
    n += n2;
    n.normalize();
    return bound(n);
}

void PxController::boundHandler(int boundary_cnt, 
        Vector2f norm1, Vector2f norm2 ,Vector2f pos) {
    int bounded;
    if (boundary_cnt == 0 || bound_locked_) {
        return;
    }else{
        if (boundary_cnt == 1) {
            bounded = bound(norm1);
        }else{
            bounded = bound(norm1,norm2);
        }
    }
    if(bounded == 0){
        setStartPoint(pos);
        boost_count_ = 0;
    }
}


void PxController::setStartPoint(Vector2f pos) {
    flight_time_ = 0;
    start_point_ = pos;
}

Vector2f PxController::controlStep(Vector2f &pos, double dt) {
    if(boost_count_ < BOOSTTIME) {
        input_ = -drift_ + start_point_ + flight_time_*v_*BOOSTPARAM;
        boost_count_++;
        std::cout << "boost :" << boost_count_ << std::endl;
    }else if (boost_count_ == BOOSTTIME){
        std::cout << "boost disabled " << std::endl;
        setStartPoint(pos);
        input_ = -drift_ + start_point_ + flight_time_*v_;
    }else{
        input_ = -drift_ + start_point_ + flight_time_*v_;
    }
    //input_ = pos - drift_ + v_;
    addTime(dt);
    checkBoundLock();
    return input_;
}

void PxController::checkBoundLock() {
    if(flight_time_ > BOUNDLOCKTIME && bound_locked_) {
        bound_locked_ = false;
        std::cout << "lock disabled" << std::endl;
    }
}

