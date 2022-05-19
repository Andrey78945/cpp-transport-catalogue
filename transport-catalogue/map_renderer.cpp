#include "map_renderer.h"

namespace renderer {

    using namespace std::literals;

    void MapRenderer::SetBusLabelFontSize(int bus_label_font_size) {
        bus_label_font_size_ = bus_label_font_size;
    }

    void MapRenderer::SetStopLabelFontSize(int stop_label_font_size) {
        stop_label_font_size_ = stop_label_font_size;
    }

    void MapRenderer::SetWidth(double width) {
        width_ = width;
    }

    void MapRenderer::SetHeight(double height) {
        height_ = height;
    }

    void MapRenderer::SetPadding(double padding) {
        padding_ = padding;
    }

    void MapRenderer::SetLineWidth(double line_width) {
        line_width_ = line_width;
    }

    void MapRenderer::SetStopRadius(double stop_radius) {
        stop_radius_ = stop_radius;
    }

    void MapRenderer::SetUnderlayerWidth(double underlayer_width) {
        underlayer_width_ = underlayer_width;
    }

    void MapRenderer::SetStopLabelOffset(double dx, double dy) {
        stop_label_offset_.first = dx;
        stop_label_offset_.second = dy;
    }

    void MapRenderer::SetBusLabelOffset(double dx, double dy) {
        bus_label_offset_.first = dx;
        bus_label_offset_.second = dy;
    }

    void MapRenderer::SetUnderlayerColor(svg::Color underlayer_color) {
        underlayer_color_ = underlayer_color;
    }

    void MapRenderer::SetColorPalette(const std::vector<svg::Color>& color_palette) {
        color_palette_ = color_palette;
    }


    int MapRenderer::GetBusLabelFontSize() const {
        return bus_label_font_size_;
    }

    int MapRenderer::GetStopLabelFontSize() const {
        return stop_label_font_size_;
    }

    double MapRenderer::GetWidth() const {
        return width_;
    }

    double MapRenderer::GetHeight() const {
        return height_;
    }

    double MapRenderer::GetPadding() const {
        return padding_;
    }

    double MapRenderer::GetLineWidth() const {
        return line_width_;
    }

    double MapRenderer::GetStopRadius() const {
        return stop_radius_;
    }

    double MapRenderer::GetUnderlayerWidth() const {
        return underlayer_width_;
    }

    const std::pair<double, double>& MapRenderer::GetStopLabelOffset() const {
        return stop_label_offset_;
    }

    const std::pair<double, double>& MapRenderer::GetBusLabelOffset() const {
        return bus_label_offset_;
    }

    const svg::Color& MapRenderer::GetUnderlayerColor() const {
        return underlayer_color_;
    }

    const std::vector<svg::Color>& MapRenderer::GetColorPalette() const {
        return color_palette_;
    }

    std::vector<svg::Polyline> MapRenderer::DrawLines(const renderer::SphereProjector& projector, 
        const transport_catalogue::TransportCatalogue& catalogue) const {

        std::vector<svg::Polyline> result;             

        size_t route_color_index = 0u;
        size_t color_palette_size = color_palette_.size();
        auto& buses = (catalogue.TransportCatalogue::GetBusesSet());
        for (const auto& bus : buses) {
            auto& stops_in_map = catalogue.TransportCatalogue::GetStopsToBuses();
            if (stops_in_map.find(bus) != stops_in_map.end()) {
                auto& stops = catalogue.TransportCatalogue::FindBus(bus)->stops;
                if (!stops.empty()) {
                    svg::Polyline polyline;
                    polyline.SetFillColor(svg::NoneColor)
                        .SetStrokeWidth(line_width_)
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
                    polyline.SetStrokeColor(color_palette_[route_color_index]);
                    route_color_index = (route_color_index < color_palette_size - 1) ? route_color_index + 1 : 0u;
                    result.push_back(std::move(polyline));
                }
            }
        }
        return result;
    }

    std::vector<svg::Text> MapRenderer::DrawRouteNames(const renderer::SphereProjector& projector, 
        const transport_catalogue::TransportCatalogue& catalogue) const {
        std::vector<svg::Text> names;
         
        size_t route_color_index = 0u;
        size_t color_palette_size = color_palette_.size();
        for (const auto& bus : catalogue.TransportCatalogue::GetBusesSet()) {
            auto& stops = catalogue.TransportCatalogue::FindBus(bus)->stops;
            if (!stops.empty()) {
                svg::Text base;
                base.SetPosition(projector((*(stops[0])).coordinates))
                    .SetOffset(svg::Point(bus_label_offset_))
                    .SetFontSize(bus_label_font_size_)
                    .SetFontFamily("Verdana"s)
                    .SetFontWeight("bold"s)
                    .SetData(catalogue.TransportCatalogue::FindBus(bus)->name);
                svg::Text sign = base
                    .SetFillColor(color_palette_[route_color_index]);
                svg::Text underlay = base
                    .SetFillColor(underlayer_color_)
                    .SetStrokeColor(underlayer_color_)
                    .SetStrokeWidth(underlayer_width_)
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

    std::vector<svg::Circle> MapRenderer::DrawStopSymbols(const renderer::SphereProjector& projector, 
        const transport_catalogue::TransportCatalogue& catalogue) const {
        std::vector<svg::Circle> symbols;
             
        auto& stops = catalogue.TransportCatalogue::GetStopsSet();

        for (const auto& stop : stops) {
            auto& buses = catalogue.TransportCatalogue::GetBusesToStops();
            if (buses.find(stop) != buses.end()) {
                svg::Circle circle;
                circle.SetCenter(projector(catalogue.TransportCatalogue::FindStop(stop)->coordinates))
                    .SetFillColor("white"s)
                    .SetRadius(stop_radius_);
                symbols.push_back(circle);
            }
        }
        return symbols;
    }

    std::vector<svg::Text> MapRenderer::DrawStopNames(const renderer::SphereProjector& projector,
        const transport_catalogue::TransportCatalogue& catalogue) const {

        std::vector<svg::Text> names;
        auto& stops = catalogue.TransportCatalogue::GetStopsSet();
        auto& buses = catalogue.TransportCatalogue::GetBusesToStops();
         
        for (const auto& stop : stops) {
            if (buses.find(stop) != buses.end()) {
                svg::Text base;
                base.SetPosition(projector(catalogue.TransportCatalogue::FindStop(stop)->coordinates))
                    .SetOffset(svg::Point(stop_label_offset_))
                    .SetFontSize(stop_label_font_size_)
                    .SetFontFamily("Verdana"s)
                    .SetData(catalogue.TransportCatalogue::FindStop(stop)->name);
                svg::Text sign = base
                    .SetFillColor("black"s);
                svg::Text underlay = base
                    .SetFillColor(underlayer_color_)
                    .SetStrokeColor(underlayer_color_)
                    .SetStrokeWidth(underlayer_width_)
                    .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                names.push_back(underlay);
                names.push_back(sign);
            }
        }
        return names;
    }

    void MapRenderer::FillDocument(svg::Document& doc, const std::vector<geo::Coordinates>& stops_on_routes, 
        const transport_catalogue::TransportCatalogue& catalogue) const {


        renderer::SphereProjector projector{ stops_on_routes.begin(), stops_on_routes.end(), width_, height_, padding_, };
        std::vector<svg::Polyline> routes = DrawLines(projector, catalogue);


        for (const auto& route : routes) {
            doc.Add(route);
        }

        std::vector<svg::Text> route_names = DrawRouteNames(projector, catalogue);
        for (const auto& name : route_names) {
            doc.Add(name);
        }

        std::vector<svg::Circle> symbols = DrawStopSymbols(projector, catalogue);
        for (const auto& symbol : symbols) {
            doc.Add(symbol);
        }

        std::vector<svg::Text> stop_names = DrawStopNames(projector, catalogue);
        for (const auto& name : stop_names) {
            doc.Add(name);
        }
    }
}