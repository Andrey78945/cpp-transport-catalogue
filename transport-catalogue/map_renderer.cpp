#include "map_renderer.h"

namespace renderer {

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
}