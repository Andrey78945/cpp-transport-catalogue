#pragma once

#include "json.h"
#include "router.h"
#include "transport_catalogue.h"
 
namespace router {    

class TransportRouter {

public:
        
    TransportRouter(int wait_time=0, double velocity=0.0);
    
    void SetWaitTime(int wait_time);
    
    void SetVelocity(double velocity);
    
    int GetWaitTime() const;

    void FillTransportRouter(const json::Dict& queryset, const transport_catalogue::TransportCatalogue& catalogue, 
        graph::DirectedWeightedGraph<double>& graph);
    
private:
    int wait_time_;
    double velocity_;
    graph::Router<double>* prouter_=nullptr;
};
}