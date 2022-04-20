#pragma once

#include "map_renderer.h"
#include "transport_catalogue.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 */

 // Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
 // с другими подсистемами приложения.
 // См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)

namespace transport_catalogue {

    //struct BusStat {};    




    class RequestHandler {
    public:

        // MapRenderer понадобится в следующей части итогового проекта
        RequestHandler(const TransportCatalogue& catalogue, const renderer::MapRenderer& renderer);
        /*
           // Возвращает информацию о маршруте (запрос Bus)
           std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

           // Возвращает маршруты, проходящие через
           const std::unordered_set<domain::Bus*>* GetBusesByStop(const std::string_view& stop_name) const;

           // Этот метод будет нужен в следующей части итогового проекта
           svg::Document RenderMap() const;
           */
        const TransportCatalogue& GetCatalogue() const;

        const renderer::MapRenderer& GetRenderer() const;

    private:
        // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
        const TransportCatalogue& catalogue_;
        const renderer::MapRenderer& renderer_;
    };

}
