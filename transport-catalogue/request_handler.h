#pragma once

#include "map_renderer.h"
#include "transport_catalogue.h"

namespace transport_catalogue {
    class RequestHandler {
    public:
        RequestHandler(const TransportCatalogue& catalogue, const renderer::MapRenderer& renderer);

        const TransportCatalogue& GetCatalogue() const;
        const renderer::MapRenderer& GetRenderer() const;

    private: 
        const TransportCatalogue& catalogue_;
        const renderer::MapRenderer& renderer_;
    };
}
