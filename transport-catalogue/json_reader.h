#pragma once

#include "json_builder.h"
#include "map_renderer.h"
#include "request_handler.h"

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
        
        void InputStops(const json::Dict& queryset, TransportCatalogue& catalogue);

        void InputBuses(const json::Dict& queryset, TransportCatalogue& catalogue);

        size_t CountUniqueStops(const std::vector<domain::Stop*>& stops);

        json::Dict GetBus(int request_id, const std::string& bus, const TransportCatalogue& catalogue);

        json::Dict GetStop(int request_id, const std::string& stop, TransportCatalogue& catalogue);

        json::Dict GetMap(int request_id, const RequestHandler& handler, const json::Dict& queryset);

        json::Dict GetNotFoundInfo(int request_id);
        
        void StatPrinter(std::ostream& out, const json::Dict& queryset, const RequestHandler& handler);

        svg::Color SetColor(const json::Node& value);
    }
}
