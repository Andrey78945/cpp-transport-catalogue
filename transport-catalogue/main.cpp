#include "json_reader.h" 
#include "map_renderer.h"
#include "request_handler.h"

#include <iostream>

int main() {
    transport_catalogue::TransportCatalogue catalogue;
    renderer::MapRenderer renderer; 
    transport_catalogue::detail::ProcessQueries(std::cin, std::cout, catalogue, renderer);
}
