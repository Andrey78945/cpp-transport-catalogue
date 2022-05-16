#include "domain.h"

namespace domain {

Stop::Stop(size_t id, geo::Coordinates coordinates, std::string name)
            : id(id)
            ,coordinates(coordinates)
            , name(name) {
        }    
}