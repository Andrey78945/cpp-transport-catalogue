#pragma once

#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

namespace transport_catalogue {
    
    class RequestHandler {
    public:

        RequestHandler(const TransportCatalogue& catalogue
            , const renderer::MapRenderer& renderer
            , const graph::Router<double>& transport_router
            , const router::TransportRouter& router);
         
        const TransportCatalogue& GetCatalogue() const;
        const renderer::MapRenderer& GetRenderer() const;
        const graph::Router<double>& GetTransportRouter() const;
        const router::TransportRouter& GetRouter() const;

    private:        
        const TransportCatalogue& catalogue_;
        const renderer::MapRenderer& renderer_;
        const graph::Router<double>& transport_router_;
        const router::TransportRouter& router_;
    };

}
