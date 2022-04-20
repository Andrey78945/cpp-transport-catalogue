#pragma once

#include "domain.h" 

#include <algorithm>
#include <cassert>
#include <cmath>
#include <deque> 
#include <execution>
#include <iostream> 
#include <map> 
#include <mutex> 
#include <set> 
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace transport_catalogue {

    class TransportCatalogue {
    public:

        struct PairHash {
            std::hash<const void*> hasher;

            size_t operator() (const std::pair<domain::Stop*, domain::Stop*>& stops_pair) const {
                size_t first = hasher(stops_pair.first);
                size_t second = hasher(stops_pair.second);
                return first + 37 * second;
            }
        };

        domain::Stop* FindStop(const std::string_view& stop);

        const domain::Stop* FindStop(const std::string_view& stop) const;

        domain::Bus* FindBus(const std::string_view& bus);

        const domain::Bus* FindBus(const std::string_view& bus) const;

        void AddStop(geo::Coordinates coordinates, const std::string& stop);

        void AddDistances(domain::Stop* from, domain::Stop* to, int distance);

        void AddBus(const std::string& bus, const std::vector<domain::Stop*>& stops, bool is_roundtrip);
        std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, int, PairHash> GetDistaces();

        std::unordered_map<std::string_view, std::vector<domain::Stop*>> GetStopsToBuses();

        const std::unordered_map<std::string_view, std::vector<domain::Stop*>>& GetStopsToBuses() const;

        std::unordered_map<std::string_view, std::vector<domain::Bus*>> GetBusesToStops();

        const std::unordered_map<std::string_view, std::vector<domain::Bus*>>& GetBusesToStops() const;

        std::unordered_map<std::string_view, domain::Stop*> GetStopsIndex();

        const std::unordered_map<std::string_view, domain::Stop*>& GetStopsIndex() const;

        std::deque<domain::Stop> GetStops();

        const std::deque<domain::Stop>& GetStops() const;

        const std::deque<domain::Bus>& GetBuses() const;

        const std::set<std::string_view>& GetBusesSet() const;

        const std::set<std::string_view>& GetStopsSet() const;

        double ComputeGeoRouteDistance(const std::vector<domain::Stop*>& stops) const;

        int ComputeRouteDistance(const std::vector<domain::Stop*>& stops) const;

    private:
        std::unordered_map<std::string_view, std::vector<domain::Bus*>> buses_to_stops_;
        std::unordered_map<std::string_view, std::vector<domain::Stop*>> stops_to_buses_;
        std::deque<domain::Bus> buses_;
        std::deque<domain::Stop> stops_;
        std::unordered_map<std::string_view, domain::Stop*> stops_index_;
        std::unordered_map<std::string_view, domain::Bus*> buses_index_;
        std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, int, PairHash> distances_;
        std::set<std::string_view> buses_set_;
        std::set<std::string_view> stops_set_;
    };
}

