#include "json_reader.h"

using namespace std::literals;

namespace transport_catalogue {
    namespace detail {
        void ProcessQueries(std::istream& input, std::ostream& output, TransportCatalogue& catalogue, renderer::MapRenderer& renderer) {
            
            json::Document doc = json::Load(input);
            auto& queryset = doc.GetRoot().AsMap();
           
            FillCatalogue(queryset, catalogue);
            
            FillMapRenderer(queryset, renderer);
            
            RequestHandler handler = RequestHandler(catalogue, renderer);
            
            StatPrinter(output, queryset, handler);   
        }

        void FillCatalogue(const json::Dict& queryset, TransportCatalogue& catalogue) {
            InputStops(queryset, catalogue);
            InputBuses(queryset, catalogue);
        }

        svg::Color SetColor(const json::Node& value) {
            if (value.IsString()) {
                return value.AsString();
            }
            else if (value.IsArray()) {
                std::vector<json::Node> colors = value.AsArray();
                if (colors.size() == 3) {
                    svg::Rgb rgb;
                    rgb.red_ = colors[0].AsInt();
                    rgb.green_ = colors[1].AsInt();
                    rgb.blue_ = colors[2].AsInt();
                    return rgb;
                }
                else {
                    svg::Rgba rgba;
                    rgba.red_ = colors[0].AsInt();
                    rgba.green_ = colors[1].AsInt();
                    rgba.blue_ = colors[2].AsInt();
                    rgba.opacity_ = colors[3].AsDouble();
                    return rgba;
                }
            }
            return "none"s;
        }

        void FillMapRenderer(const json::Dict& queryset, renderer::MapRenderer& renderer) {            
            if (auto it = queryset.find("render_settings"s); it == queryset.end()) {
                return;
            }
            for (const auto& [key, value] : queryset.at("render_settings"s).AsMap()) {
                if (key == "width"s) {
                    renderer.SetWidth(value.AsDouble());
                }
                else if (key == "height"s) {
                    renderer.SetHeight(value.AsDouble());
                }
                else if (key == "padding"s) {
                    renderer.SetPadding(value.AsDouble());
                }
                else if (key == "stop_radius"s) {
                    renderer.SetStopRadius(value.AsDouble());
                }
                else if (key == "line_width"s) {
                    renderer.SetLineWidth(value.AsDouble());
                }
                else if (key == "bus_label_font_size"s) {
                    renderer.SetBusLabelFontSize(value.AsInt());
                }
                else if (key == "bus_label_offset"s) {
                    auto array = value.AsArray();
                    renderer.SetBusLabelOffset(array[0].AsDouble(), array[1].AsDouble());
                }
                else if (key == "stop_label_font_size"s) {
                    renderer.SetStopLabelFontSize(value.AsInt());
                }
                else if (key == "stop_label_offset"s) {
                    auto array = value.AsArray();
                    renderer.SetStopLabelOffset(array[0].AsDouble(), array[1].AsDouble());
                }
                else if (key == "underlayer_width"s) {
                    renderer.SetUnderlayerWidth(value.AsDouble());
                }
                else if (key == "underlayer_color"s) {
                    renderer.SetUnderlayerColor(SetColor(value));
                }
                else if (key == "color_palette"s) {
                    std::vector<svg::Color> palette;
                    for (const auto& color : value.AsArray()) {
                        palette.push_back(SetColor(color));
                    }
                    renderer.SetColorPalette(palette);
                }
            }
        }

        void InputStops(const json::Dict& queryset, TransportCatalogue& catalogue) {
            for (const auto& query : queryset.at("base_requests"s).AsArray()) {
                auto map = query.AsMap();
                if (map.at("type"s).AsString() == "Stop") {
                    double lat = map.at("latitude"s).AsDouble();
                    double lng = map.at("longitude"s).AsDouble();
                    std::string name = map.at("name"s).AsString();
                    catalogue.TransportCatalogue::AddStop(geo::Coordinates{ lat, lng }, name);
                }
            }
            for (const auto& query : queryset.at("base_requests"s).AsArray()) {
                auto map = query.AsMap();
                if (map.at("type"s).AsString() == "Stop") {
                    std::string name = map.at("name"s).AsString();
                    auto distances = map.at("road_distances"s).AsMap();
                    if (!distances.empty()) {
                        for (const auto& [to, distance] : distances) {
                            domain::Stop* from = catalogue.TransportCatalogue::FindStop(name);
                            catalogue.TransportCatalogue::AddDistances(from, catalogue.TransportCatalogue::FindStop(to), distance.AsInt());
                        }
                    }
                }
            }
        }

        void InputBuses(const json::Dict& queryset, TransportCatalogue& catalogue) {
            for (const auto& query : queryset.at("base_requests"s).AsArray()) {
                auto map = query.AsMap();
                if (map.at("type"s).AsString() == "Bus") {
                    bool is_roundtrip = map.at("is_roundtrip"s).AsBool();
                    std::vector<domain::Stop*> stops;
                    std::string name = map.at("name"s).AsString();
                    for (const auto& stop : map.at("stops"s).AsArray()) {
                        stops.push_back(catalogue.TransportCatalogue::FindStop(stop.AsString()));
                    }
                    catalogue.TransportCatalogue::AddBus(name, stops, is_roundtrip);
                }
            }
        }

        size_t CountUniqueStops(const std::vector<domain::Stop*>& stops) {
            std::set<std::string> counts;
            for (const domain::Stop* stop : stops) {
                counts.insert((*stop).name);
            }
            return counts.size();
        }

        json::Dict GetNotFoundInfo(int request_id) {
            return json::Dict{ {"request_id"s, json::Node(request_id)}, {"error_message"s, json::Node("not found"s)} };
        }

        json::Dict GetBus(int request_id, const std::string& bus, const TransportCatalogue& catalogue) {
            if (catalogue.TransportCatalogue::GetStopsToBuses().count(bus) == 0) {
                return GetNotFoundInfo(request_id);
            }
            std::vector<domain::Stop*> stops = catalogue.TransportCatalogue::GetStopsToBuses().at(bus);
            int stops_on_route;
            int unique_stops = CountUniqueStops(stops);
            const domain::Bus* pbus = catalogue.TransportCatalogue::FindBus(bus);
            double route_geo_length = catalogue.ComputeGeoRouteDistance(stops);
            int route_length = catalogue.ComputeRouteDistance(stops);
            double route_curvature = 0.0;
            if ((*pbus).is_roundtrip) {
                stops_on_route = stops.size();
                route_curvature = route_length / route_geo_length;
            }
            else {
                stops_on_route = stops.size() * 2 - 1;
                std::reverse(stops.begin(), stops.end());
                route_length += catalogue.ComputeRouteDistance(stops);
                route_curvature = route_length / route_geo_length / 2.0;
            }
            return json::Dict{ {"curvature"s, json::Node(route_curvature)}, {"request_id"s, json::Node(request_id)}, {"route_length"s, json::Node(route_length)}, {"stop_count"s, json::Node(stops_on_route)}, {"unique_stop_count"s, json::Node(unique_stops)} };
        }

        json::Dict GetStop(int request_id, const std::string& stop, const TransportCatalogue& catalogue) {
            if (catalogue.TransportCatalogue::GetStopsIndex().count(stop) == 0) {
                return GetNotFoundInfo(request_id);
            }
            std::vector<domain::Bus*> buses;
            buses = (catalogue.TransportCatalogue::GetBusesToStops().count(stop) > 0) ? catalogue.TransportCatalogue::GetBusesToStops().at(stop) : buses;
            std::set<std::string> routes;
            for (const auto bus : buses) {
                routes.insert((*bus).name);
            }
            json::Array buses_on_stop;
            for (const auto& bus : routes) {
                buses_on_stop.push_back(json::Node(bus));
            } 
            return json::Dict{ {"buses"s, buses_on_stop}, {"request_id"s, json::Node(request_id)} }; ;
        }

        std::vector<geo::Coordinates> FindStopsOnRoutes(const RequestHandler& handler) {
            std::vector<geo::Coordinates> stops_on_routes;
            for (const auto& bus : handler.RequestHandler::GetCatalogue().GetBuses()) {
                for (const auto& stop : bus.stops) {
                    stops_on_routes.push_back((*stop).coordinates);
                }
            }
            return stops_on_routes;
        }

        std::vector<svg::Polyline> DrawLines(const renderer::SphereProjector& projector, const RequestHandler& handler) {
            std::vector<svg::Polyline> result;

            auto catalogue = handler.RequestHandler::GetCatalogue();
            auto render = handler.RequestHandler::GetRenderer();

            size_t route_color_index = 0u;
            size_t color_palette_size = render.GetColorPalette().size();
            auto buses = (catalogue.TransportCatalogue::GetBusesSet());
            for (const auto& bus : buses) { 
                auto stops_in_map = catalogue.TransportCatalogue::GetStopsToBuses();
                if (stops_in_map.find(bus) != stops_in_map.end()) {
                    auto stops = catalogue.TransportCatalogue::FindBus(bus)->stops;
                        if (!stops.empty()) {
                            svg::Polyline polyline;
                            polyline.SetFillColor(svg::NoneColor)
                                .SetStrokeWidth(render.GetLineWidth())
                                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                            for (const auto& stop : stops) { 
                                polyline.AddPoint(projector((*stop).coordinates));
                            }
                            
                            auto b = catalogue.TransportCatalogue::FindBus(bus);
                            if (!(b->is_roundtrip) && stops.size() > 1u) {
                                for (auto i = 1u; i < stops.size(); ++i) {
                                    polyline.AddPoint(projector((*(stops[stops.size() - i - 1])).coordinates));
                                }
                            }
                            polyline.SetStrokeColor(render.GetColorPalette()[route_color_index]);
                            route_color_index = (route_color_index < color_palette_size - 1) ? route_color_index + 1 : 0u;
                            result.push_back(std::move(polyline));
                        }                        
                }               
            }
            return result;
        }

        std::vector<svg::Text> DrawRouteNames(const renderer::SphereProjector & projector, const RequestHandler & handler) {
            std::vector<svg::Text> names;
            auto catalogue = handler.RequestHandler::GetCatalogue();
            auto render = handler.RequestHandler::GetRenderer();
            size_t route_color_index = 0u;
            size_t color_palette_size = render.GetColorPalette().size();
            for (const auto& bus : catalogue.TransportCatalogue::GetBusesSet()) {
                auto stops = catalogue.TransportCatalogue::FindBus(bus)->stops;
                if (!stops.empty()) { 
                    svg::Text base;
                    base.SetPosition(projector((*(stops[0])).coordinates))
                        .SetOffset(svg::Point(render.GetBusLabelOffset()))
                        .SetFontSize(render.GetBusLabelFontSize())
                        .SetFontFamily("Verdana"s)
                        .SetFontWeight("bold"s)
                        .SetData(catalogue.TransportCatalogue::FindBus(bus)->name);
                    svg::Text sign = base
                        .SetFillColor(render.GetColorPalette()[route_color_index]);
                    svg::Text underlay = base
                        .SetFillColor(render.GetUnderlayerColor())
                        .SetStrokeColor(render.GetUnderlayerColor())
                        .SetStrokeWidth(render.GetUnderlayerWidth())
                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);                    
                    names.push_back(underlay);
                    names.push_back(sign);
                     
                    if (!(catalogue.TransportCatalogue::FindBus(bus)->is_roundtrip) && stops.size() > 1u && stops[0]->name != stops[stops.size() - 1]->name) {
                        underlay.SetPosition(projector((*(stops[stops.size() - 1])).coordinates));
                        sign.SetPosition(projector((*(stops[stops.size() - 1])).coordinates));
                        names.push_back(std::move(underlay));
                        names.push_back(std::move(sign));
                    }                    
                    route_color_index = (route_color_index < color_palette_size - 1) ? route_color_index + 1 : 0u;                    
                }
            }
            return names;
        }

        std::vector<svg::Circle> DrawStopSymbols(const renderer::SphereProjector & projector, const RequestHandler & handler) {
            std::vector<svg::Circle> symbols;
            auto catalogue = handler.RequestHandler::GetCatalogue();
            auto render = handler.RequestHandler::GetRenderer();
            auto stops = catalogue.TransportCatalogue::GetStopsSet();
            
            for (const auto& stop : stops) {                
                auto buses = catalogue.TransportCatalogue::GetBusesToStops();
                if (buses.find(stop) != buses.end()) {
                    svg::Circle circle;
                    circle.SetCenter(projector(catalogue.TransportCatalogue::FindStop(stop)->coordinates))
                        .SetFillColor("white"s)
                        .SetRadius(render.GetStopRadius());
                    symbols.push_back(circle);
                }
            }
            return symbols;
        }

        std::vector<svg::Text> DrawStopNames(const renderer::SphereProjector& projector, const RequestHandler& handler) {
            std::vector<svg::Text> names;
            auto stops = handler.RequestHandler::GetCatalogue().TransportCatalogue::GetStopsSet(); 
            auto buses = handler.RequestHandler::GetCatalogue().TransportCatalogue::GetBusesToStops();
            auto catalogue = handler.RequestHandler::GetCatalogue();
            auto render = handler.RequestHandler::GetRenderer();
            for (const auto& stop : stops) {  
                if (buses.find(stop) != buses.end()) {
                    svg::Text base;
                    base.SetPosition(projector(catalogue.TransportCatalogue::FindStop(stop)->coordinates))
                        .SetOffset(svg::Point(render.GetStopLabelOffset()))
                        .SetFontSize(render.GetStopLabelFontSize())
                        .SetFontFamily("Verdana"s) 
                        .SetData(catalogue.TransportCatalogue::FindStop(stop)->name);
                    svg::Text sign = base
                        .SetFillColor("black"s);
                    svg::Text underlay = base
                        .SetFillColor(render.GetUnderlayerColor())
                        .SetStrokeColor(render.GetUnderlayerColor())
                        .SetStrokeWidth(render.GetUnderlayerWidth())
                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                    names.push_back(underlay);
                    names.push_back(sign);
                }
            }
            return names;
        }
        

        json::Dict GetMap(int request_id, const RequestHandler& handler, const json::Dict& queryset) {
            if (auto it = queryset.find("render_settings"s); it == queryset.end()) {
                return GetNotFoundInfo(request_id);
            }
            std::vector<geo::Coordinates> stops_on_routes = FindStopsOnRoutes(handler);
            renderer::SphereProjector projector{ stops_on_routes.begin(), stops_on_routes.end(), handler.RequestHandler::GetRenderer().MapRenderer::GetWidth(), handler.RequestHandler::GetRenderer().MapRenderer::GetHeight(), handler.RequestHandler::GetRenderer().MapRenderer::GetPadding(), };
            std::vector<svg::Polyline> routes = DrawLines(projector, handler);
            
            svg::Document doc;
            for (const auto route : routes) {
                doc.Add(route);
            }

            std::vector<svg::Text> route_names = DrawRouteNames(projector, handler);
            for (const auto name : route_names) {
                doc.Add(name);
            }

            std::vector<svg::Circle> symbols = DrawStopSymbols(projector, handler);
            for (const auto symbol : symbols) {
                doc.Add(symbol);
            }

            std::vector<svg::Text> stop_names = DrawStopNames(projector, handler);
            for (const auto name : stop_names) {
                doc.Add(name);
            }

            std::ostringstream ss; 
            doc.Render(ss).rdbuf();
            std::string routes_map = ss.str();
            return json::Dict{ {"map"s, json::Node(routes_map)}, {"request_id"s, json::Node(request_id)} };
        }

        void StatPrinter(std::ostream& out, const json::Dict& queryset, const RequestHandler& handler)
        {
            
            json::Array stat_printer;

            for (const auto& request : queryset.at("stat_requests"s).AsArray()) {
                auto map = request.AsMap();
                std::string type = map.at("type"s).AsString();
                int request_id = map.at("id"s).AsInt();                
                if (type == "Bus"s) {
                    std::string name = map.at("name"s).AsString();
                    stat_printer.push_back(GetBus(request_id, name, handler.RequestHandler::GetCatalogue()));
                }
                else if (type == "Stop"s) {
                    std::string name = map.at("name"s).AsString();
                    stat_printer.push_back(GetStop(request_id, name, handler.RequestHandler::GetCatalogue()));
                }
                else if (type == "Map"s) {
                    
                    stat_printer.push_back(GetMap(request_id, handler, queryset));  
                }
            }
            
            json::Print(json::Document(stat_printer), out);
        }
    }
}

