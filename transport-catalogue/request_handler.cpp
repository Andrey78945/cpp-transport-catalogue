#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */
namespace transport_catalogue {

    RequestHandler::RequestHandler(const TransportCatalogue& catalogue, const renderer::MapRenderer& renderer)
        : catalogue_(catalogue)
        , renderer_(renderer) {

    }
    /*
      //   Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
        return BusStat{bus_name};
    }

        // Возвращает маршруты, проходящие через
        const std::unordered_set<domain::Bus*>* RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
            return {};
        }

        // Этот метод будет нужен в следующей части итогового проекта
        svg::Document RequestHandler::RenderMap() const {
            return svg::Document{};
        }
    */
    const TransportCatalogue& RequestHandler::GetCatalogue() const {
        return catalogue_;
    }

    const renderer::MapRenderer& RequestHandler::GetRenderer() const {
        return renderer_;
    }

}