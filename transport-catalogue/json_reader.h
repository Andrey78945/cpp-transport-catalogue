#pragma once

#include "json_builder.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"


#include <cstdlib> 
#include <deque> 
#include <iomanip> 
#include <iostream> 
#include <map>
#include <set> 
#include <string> 
#include <string_view>
#include <tuple> 
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace transport_catalogue {
    namespace detail {

        void ProcessQueries(std::istream& input, std::ostream& output, TransportCatalogue& catalogue, renderer::MapRenderer& renderer);

        void FillCatalogue(const json::Dict& queryset, TransportCatalogue& catalogue);

        void FillMapRenderer(const json::Dict& queryset, renderer::MapRenderer& renderer);       
        
        void FillTransportRouter(const json::Dict& queryset, const TransportCatalogue& catalogue, graph::DirectedWeightedGraph<double>& graph, router::TransportRouter& router);

        void InputStops(const json::Dict& queryset, TransportCatalogue& catalogue);

        void InputBuses(const json::Dict& queryset, TransportCatalogue& catalogue);

        size_t CountUniqueStops(const std::vector<domain::Stop*>& stops);

        json::Dict GetBus(int request_id, const std::string& bus, const TransportCatalogue& catalogue);

        json::Dict GetStop(int request_id, const std::string& stop, TransportCatalogue& catalogue);

        std::vector<geo::Coordinates> FindStopsOnRoutes(const RequestHandler& handler);

        json::Dict GetMap(int request_id, const RequestHandler& handler, const json::Dict& queryset);

        json::Dict GetNotFoundInfo(int request_id);

        std::vector<svg::Polyline> DrawLines(const renderer::SphereProjector& projector, const RequestHandler& handler);

        std::vector<svg::Text> DrawRouteNames(const renderer::SphereProjector& projector, const RequestHandler& handler);

        std::vector<svg::Circle> DrawStopSymbols(const renderer::SphereProjector & projector, const RequestHandler & handler);

        void StatPrinter(std::ostream& out, const json::Dict& queryset, const RequestHandler& handler);

        svg::Color SetColor(const json::Node& value);
    }
}
