#pragma once

#include "router.h"

 
namespace router {
    

class TransportRouter {

public:
        
    TransportRouter(int wait_time=0, double velocity=0.0);
    
    void SetWaitTime(int wait_time);
    
    void SetVelocity(double velocity);
    
    int GetWaitTime() const;
    
private:
    int wait_time_;
    double velocity_;
    graph::Router<double>* prouter_=nullptr;
};
}