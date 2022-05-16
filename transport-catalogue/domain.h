#pragma once

#include "geo.h"

#include <string> 
#include <vector>

namespace domain {
    struct Stop {
        size_t id;
        geo::Coordinates coordinates;
        std::string name;

        Stop(size_t id, geo::Coordinates coordinates, std::string name);
    };

    struct Bus {
        bool is_roundtrip = false;
        std::vector<Stop*> stops;
        std::string name;
    };
}
  