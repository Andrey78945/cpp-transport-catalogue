#pragma once

#include "domain.h"
#include "geo.h"
#include "svg.h"
#include "transport_catalogue.h"

#include <algorithm>
#include <optional>
#include <variant>
#include <vector>

namespace renderer {

    inline const double EPSILON = 1e-6;
    inline bool IsZero(double value) { return std::abs(value) < EPSILON; }

    class SphereProjector {
    public:
        template <typename PointInputIt>
        SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding)
            : padding_(padding) {
            if (points_begin == points_end) {
                return;
            }
            const auto [left_it, right_it]
                = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
                return lhs.lng < rhs.lng; });
            min_lon_ = left_it->lng;
            const double max_lon = right_it->lng;

            const auto [bottom_it, top_it]
                = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
                return lhs.lat < rhs.lat; });
            const double min_lat = bottom_it->lat;
            max_lat_ = top_it->lat;

            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_)) {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat)) {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom) {
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            }
            else if (width_zoom) {
                zoom_coeff_ = *width_zoom;
            }
            else if (height_zoom) {
                zoom_coeff_ = *height_zoom;
            }
        }

        svg::Point operator()(geo::Coordinates coords) const {
            return { (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                    (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
        }
    private:
        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };

    class MapRenderer {
    public:
        void SetBusLabelFontSize(int bus_label_font_size); 

        void SetStopLabelFontSize(int stop_label_font_size);

        void SetWidth(double width);

        void SetHeight(double height);

        void SetPadding(double padding);

        void SetLineWidth(double line_width);

        void SetStopRadius(double stop_radius);

        void SetUnderlayerWidth(double underlayer_width);

        void SetStopLabelOffset(double dx, double dy);

        void SetBusLabelOffset(double dx, double dy);

        void SetUnderlayerColor(svg::Color underlayer_color);

        void SetColorPalette(const std::vector<svg::Color>& color_palette);


        int GetBusLabelFontSize() const;

        int GetStopLabelFontSize() const;

        double GetWidth() const;

        double GetHeight() const;

        double GetPadding() const;

        double GetLineWidth() const;

        double GetStopRadius() const;

        double GetUnderlayerWidth() const;

        const std::pair<double, double>& GetStopLabelOffset() const;

        const std::pair<double, double>& GetBusLabelOffset() const;

        const svg::Color& GetUnderlayerColor() const;

        const std::vector<svg::Color>& GetColorPalette() const;

        std::vector<svg::Polyline> DrawLines(const renderer::SphereProjector& projector, const transport_catalogue::TransportCatalogue& catalogue) const;

        std::vector<svg::Text> DrawRouteNames(const renderer::SphereProjector& projector, const transport_catalogue::TransportCatalogue& catalogue) const;

        std::vector<svg::Circle> DrawStopSymbols(const renderer::SphereProjector& projector, const transport_catalogue::TransportCatalogue& catalogue) const;

        std::vector<svg::Text> DrawStopNames(const renderer::SphereProjector& projector, const transport_catalogue::TransportCatalogue& catalogue) const;
        
        void FillDocument(svg::Document& doc, const std::vector<geo::Coordinates>& stops_on_routes, 
            const transport_catalogue::TransportCatalogue& catalogue) const;


    private: 
        int bus_label_font_size_ = 0;
        int stop_label_font_size_ = 0;
        double width_ = 0.0;
        double height_ = 0.0;
        double padding_ = 0.0;
        double line_width_ = 0.0;
        double stop_radius_ = 0.0;
        double underlayer_width_ = 0.0;
        std::pair<double, double> stop_label_offset_{ 0.0, 0.0 };
        std::pair<double, double> bus_label_offset_{ 0.0, 0.0 };
        svg::Color underlayer_color_ = "none";
        std::vector<svg::Color> color_palette_;
    };
}