#include "transport_catalogue.h"

namespace transport_catalogue {


    domain::Stop* TransportCatalogue::FindStop(const std::string_view& stop) {
        return stops_index_[stop];
    }

    const domain::Stop* TransportCatalogue::FindStop(const std::string_view& stop) const {
        return stops_index_.at(stop);
    }

    void TransportCatalogue::AddStop(geo::Coordinates coordinates, const std::string& name) {
        stops_.push_back(std::move(domain::Stop(stops_.size(), coordinates, name)));
        domain::Stop* pstop = &(stops_.back());
        std::string_view stop_view{ (*pstop).name };
        stops_index_[stop_view] = pstop;
        stops_set_.insert(stop_view);
    }

    void TransportCatalogue::AddDistances(domain::Stop* from, domain::Stop* to, int distance)
    {
        std::pair<domain::Stop*, domain::Stop*> stops(from, to);
        distances_[stops] = distance;
    }

    void TransportCatalogue::AddBus(const std::string& name, const std::vector<domain::Stop*>& stops, bool is_roundtrip)
    {
        domain::Bus new_bus{ is_roundtrip, stops, name };
        buses_.push_back(std::move(new_bus));
        domain::Bus* pbus = &(buses_.back());        
        std::string_view bus_view{ (*pbus).name };
        buses_index_[bus_view] = pbus;
        buses_set_.insert(bus_view);
        std::vector<domain::Stop*> stops_in_map;
        for (const auto& stop : stops) {
            std::string_view stop_view{ (*stop).name };
            buses_to_stops_[stop_view].push_back(pbus);
        }
        stops_to_buses_[bus_view] = stops;
    }

    domain::Bus* TransportCatalogue::FindBus(const std::string_view& bus) {
        return  buses_index_[bus];
    }

    const domain::Bus* TransportCatalogue::FindBus(const std::string_view& bus) const {
        return  buses_index_.at(bus);
    }

    std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, int, TransportCatalogue::PairHash> TransportCatalogue::GetDistances() {
        return distances_;
    }

    const std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, int, TransportCatalogue::PairHash>& TransportCatalogue::GetDistances() const {
        return distances_;
    }

    std::unordered_map<std::string_view, std::vector<domain::Stop*>> TransportCatalogue::GetStopsToBuses() {
        return stops_to_buses_;
    }

    const std::unordered_map<std::string_view, std::vector<domain::Stop*>>& TransportCatalogue::GetStopsToBuses() const {
        return stops_to_buses_;
    }

    std::unordered_map<std::string_view, std::vector<domain::Bus*>> TransportCatalogue::GetBusesToStops() {
        return buses_to_stops_;
    }

    const std::unordered_map<std::string_view, std::vector<domain::Bus*>>& TransportCatalogue::GetBusesToStops() const {
        return buses_to_stops_;
    }

    std::unordered_map<std::string_view, domain::Stop*> TransportCatalogue::GetStopsIndex() {
        return stops_index_;
    }

    const std::unordered_map<std::string_view, domain::Stop*>& TransportCatalogue::GetStopsIndex() const {
        return stops_index_;
    }

    std::deque<domain::Stop> TransportCatalogue::GetStops() {
        return stops_;
    }

    const std::deque<domain::Stop>& TransportCatalogue::GetStops() const {
        return stops_;
    }

    const std::deque<domain::Bus>& TransportCatalogue::GetBuses() const {
        return buses_;
    }

    const std::set<std::string_view>& TransportCatalogue::GetBusesSet() const {
        return buses_set_;
    }

    const std::set<std::string_view>& TransportCatalogue::GetStopsSet() const {
        return stops_set_;
    }

    double TransportCatalogue::ComputeGeoRouteDistance(const std::vector<domain::Stop*>& stops) const
    {
        assert(stops.size() >= 2);
        double route_geo_distance = 0.0;
        for (size_t i = 1; i < stops.size(); ++i) {
            route_geo_distance += ComputeDistance((*stops[i - 1]).coordinates, (*stops[i]).coordinates);
        }
        return route_geo_distance;
    }

    int TransportCatalogue::ComputeRouteDistance(const std::vector<domain::Stop*>& stops) const
    {
        assert(stops.size() >= 2);
        int route_data_distance = 0;
        for (size_t i = 1; i < stops.size(); ++i) {
            auto it = distances_.find({ stops[i - 1], stops[i] });
            if (it == distances_.end()) {
                it = distances_.find({ stops[i], stops[i - 1] });
            }
            route_data_distance += it->second;
        }
        return route_data_distance;
    }
}
