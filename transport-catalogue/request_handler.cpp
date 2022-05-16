#include "request_handler.h"
 
namespace transport_catalogue {

    RequestHandler::RequestHandler(const TransportCatalogue& catalogue
        , const renderer::MapRenderer& renderer
        , const graph::Router<double>& transport_router
        , const router::TransportRouter& router)
        : catalogue_(catalogue)
        , renderer_(renderer)
        , transport_router_(transport_router)
        , router_(router) {

    }
    
    const TransportCatalogue& RequestHandler::GetCatalogue() const {
        return catalogue_;
    }

    const renderer::MapRenderer& RequestHandler::GetRenderer() const {
        return renderer_;
    }
    
    const graph::Router<double>& RequestHandler::GetTransportRouter() const {
        return transport_router_;
    }

    const router::TransportRouter& RequestHandler::GetRouter() const {
        return router_;
    }
}