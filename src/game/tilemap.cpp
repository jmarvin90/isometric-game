#include "spdlog/spdlog.h"

#include "tilemap.h"
#include "constants.h"


TileMap::TileMap(entt::registry& registry) {
    spdlog::info("TileMap constructor called.");
    for (int x=0; x<constants::MAP_SIZE; x++) {
        std::vector<entt::entity> row;
        for (int y=0; y<constants::MAP_SIZE; y++) {
            row.push_back(registry.create());
        }
        tilemap.push_back(row);
    }
}

TileMap::~TileMap() {
    spdlog::info("TileMap destructor called.");
}

entt::entity TileMap::at(const int x, const int y) {
    return tilemap.at(x).at(y);
}