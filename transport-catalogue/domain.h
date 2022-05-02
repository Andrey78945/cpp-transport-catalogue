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
} 