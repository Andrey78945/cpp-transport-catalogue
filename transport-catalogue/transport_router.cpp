#include "transport_router.h"

namespace router {
    
    TransportRouter::TransportRouter(int wait_time, double velocity)
        : wait_time_(wait_time)
        , velocity_(velocity) {
            
        }
    
    void TransportRouter::SetWaitTime(int wait_time) {
        wait_time_ = wait_time;
    }
    
    void TransportRouter::SetVelocity(double velocity) {
        velocity_ = velocity;
    }
    
    int TransportRouter::GetWaitTime() const {
        return wait_time_;
    }
}