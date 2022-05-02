#include "request_handler.h"

namespace transport_catalogue {

    RequestHandler::RequestHandler(const TransportCatalogue& catalogue, const renderer::MapRenderer& renderer)
        : catalogue_(catalogue)
        , renderer_(renderer) {
    }
    
    const TransportCatalogue& RequestHandler::GetCatalogue() const {
        return catalogue_;
    }

    const renderer::MapRenderer& RequestHandler::GetRenderer() const {
        return renderer_;
    }
}