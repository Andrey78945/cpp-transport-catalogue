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

    void TransportRouter::FillTransportRouter(const json::Dict& queryset, const transport_catalogue::TransportCatalogue& catalogue,
        graph::DirectedWeightedGraph<double>& graph) {
        using namespace std::literals;

        if (auto it = queryset.find("routing_settings"s); it == queryset.end()) {
            return;
        }
        int bus_wait_time = 0;
        double velocity = 0.0;

        for (const auto& [key, value] : queryset.at("routing_settings"s).AsDict()) {
            if (key == "bus_velocity"s) {
                velocity = value.AsDouble();
                SetVelocity(velocity);
            }
            else if (key == "bus_wait_time"s) {
                bus_wait_time = value.AsInt();
                SetWaitTime(bus_wait_time);
            }
        }
        for (const auto& bus : catalogue.TransportCatalogue::GetBuses()) {
            const auto& stops = bus.stops;
            size_t span = 0;
            double weight = bus_wait_time * 1.0;
            if (stops.size() > 1) {
                for (size_t i = 0; i + 1 < stops.size(); ++i) {
                    span = 1;
                    weight = bus_wait_time * 1.0;
                    for (size_t j = i + 1; j < stops.size(); ++j) {

                        if (stops[i] != stops[j]) {
                            auto it = catalogue.TransportCatalogue::GetDistances().find({ stops[j - 1], stops[j] });
                            if (it == catalogue.TransportCatalogue::GetDistances().end()) {
                                it = catalogue.TransportCatalogue::GetDistances().find({ stops[j], stops[j - 1] });
                            }
                            weight += it->second / 1000.0 / velocity * 60.0;
                            graph.AddEdge({ stops[i]->id, stops[j]->id, span, bus.name, weight });
                            ++span;
                        }
                    }
                }
                if (!bus.is_roundtrip) {
                    for (size_t i = stops.size() - 1u; i > 0; --i) {
                        weight = bus_wait_time * 1.0;
                        span = 1;
                        for (size_t j = i; j > 0; --j) {

                            if (stops[i] != stops[j - 1]) {
                                auto it = catalogue.TransportCatalogue::GetDistances().find({ stops[j], stops[j - 1] });
                                if (it == catalogue.TransportCatalogue::GetDistances().end()) {
                                    it = catalogue.TransportCatalogue::GetDistances().find({ stops[j - 1], stops[j] });
                                }
                                weight += it->second / 1000.0 / velocity * 60.0;
                                graph.AddEdge({ stops[i]->id, stops[j - 1]->id, span, bus.name, weight });
                                ++span;
                            }
                        }
                    }
                }
            }
        }
    }
}
