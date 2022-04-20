#pragma once

#include "domain.h"
#include "geo.h"
#include "svg.h"

#include <algorithm>
#include <optional>
#include <variant>
#include <vector>

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 */

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
        void SetBusLabelFontSize(int bus_label_font_size) {
            bus_label_font_size_ = bus_label_font_size;
        }

        void SetStopLabelFontSize(int stop_label_font_size) {
            stop_label_font_size_ = stop_label_font_size;
        }

        void SetWidth(double width) {
            width_ = width;
        }

        void SetHeight(double height) {
            height_ = height;
        }

        void SetPadding(double padding) {
            padding_ = padding;
        }

        void SetLineWidth(double line_width) {
            line_width_ = line_width;
        }

        void SetStopRadius(double stop_radius) {
            stop_radius_ = stop_radius;
        }

        void SetUnderlayerWidth(double underlayer_width) {
            underlayer_width_ = underlayer_width;
        }

        void SetStopLabelOffset(double dx, double dy) {
            stop_label_offset_.first = dx;
            stop_label_offset_.second = dy;
        }

        void SetBusLabelOffset(double dx, double dy) {
            bus_label_offset_.first = dx;
            bus_label_offset_.second = dy;
        }

        void SetUnderlayerColor(svg::Color underlayer_color) {
            underlayer_color_ = underlayer_color;
        }

        void SetColorPalette(const std::vector<svg::Color>& color_palette) {
            color_palette_ = color_palette;
        }


        int GetBusLabelFontSize() const {
            return bus_label_font_size_;
        }

        int GetStopLabelFontSize() const {
            return stop_label_font_size_;
        }

        double GetWidth() const {
            return width_;
        }

        double GetHeight() const {
            return height_;
        }

        double GetPadding() const {
            return padding_;
        }

        double GetLineWidth() const {
            return line_width_;
        }

        double GetStopRadius() const {
            return stop_radius_;
        }

        double GetUnderlayerWidth() const {
            return underlayer_width_;
        }

        const std::pair<double, double>& GetStopLabelOffset() const {
            return stop_label_offset_;
        }

        const std::pair<double, double>& GetBusLabelOffset() const {
            return bus_label_offset_;
        }

        const svg::Color& GetUnderlayerColor() const {
            return underlayer_color_;
        }

        const std::vector<svg::Color>& GetColorPalette() const {
            return color_palette_;
        }

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