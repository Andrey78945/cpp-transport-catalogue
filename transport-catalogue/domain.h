#pragma once

#include "geo.h"

#include <string>
#include <variant>
#include <vector>

namespace domain {
    struct Stop {
        geo::Coordinates coordinates;
        std::string name;

        Stop(geo::Coordinates coordinates, std::string name)
            :coordinates(coordinates)
            , name(name) {
        }
    };

    struct Bus {
        bool is_roundtrip = false;
        std::vector<Stop*> stops;
        std::string name;
    };

    /*    struct Rgb {
        Rgb() = default;
        Rgb(int red, int green, int blue)
            : red_(red)
            , green_(green)
            , blue_(blue) {
            }

        int red_ = 0;
        int green_ = 0;
        int blue_ = 0;
    };

    struct Rgba {
        Rgba() = default;
        Rgba(int red, int green, int blue, double opacity)
            : red_(red)
            , green_(green)
            , blue_(blue)
            , opacity_(opacity) {
            }

        int red_ = 0;
        int green_ = 0;
        int blue_ = 0;
        double opacity_ = 1.0;
    };

        using Color = std::variant<std::string, Rgb, Rgba>;
      */
}
/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */