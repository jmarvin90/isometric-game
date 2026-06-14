#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL2/SDL.h>
#include <array>
#include <directions.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

// TODO - probably move gates / translations
namespace {
struct Gate {
    glm::ivec2 entry;
    glm::ivec2 exit;
};

// TODO - investigate whether this is correct - comments or otherwise
inline constexpr std::array<Gate, 4> translations {
    {
        { { 2, -1 }, { 1, -2 } }, // North
        { { -1, -2 }, { -2, -1 } }, // East
        { { -2, 1 }, { -1, 2 } }, // South
        { { 1, 2 }, { 2, 1 } } // West
    }
};
}

namespace Constants {
inline constexpr glm::ivec2 TILE_SIZE { 256, 128 };
inline constexpr glm::ivec2 TILE_CENTRE { TILE_SIZE / 2 };
inline constexpr glm::ivec2 CAMERA_SCROLL_SPEED { 18, 9 };
inline constexpr int FPS { 60 };
inline constexpr float MILLIS_PER_FRAME { 1'000.f / FPS };
inline constexpr glm::ivec2 SCENE_BORDER_PX { 150, 150 };
inline constexpr int ROAD_WIDTH_PX { 68 };
// inline constexpr glm::ivec2 ROAD_MARK_OFFSET { ROAD_WIDTH_PX / 4, ROAD_WIDTH_PX / 8 };
inline constexpr glm::ivec2 ROAD_MARK_OFFSET { 20, 10 };
const std::string spritesheet { "assets/spritesheet_scaled.png" };
const std::string SAVE_FILE_PATH { "save.json" };

inline std::unordered_map<Direction::TDirection, std::string> WALKER_DIRECTIONS //
    { { { Direction::TDirection::NORTH, "walker_n" },
        { Direction::TDirection::NORTH_WEST, "walker_nw" },
        { Direction::TDirection::WEST, "walker_w" },
        { Direction::TDirection::SOUTH_WEST, "walker_sw" },
        { Direction::TDirection::SOUTH, "walker_s" },
        { Direction::TDirection::SOUTH_EAST, "walker_se" },
        { Direction::TDirection::EAST, "walker_e" },
        { Direction::TDirection::NORTH_EAST, "walker_ne" } } };

inline constexpr std::array<SDL_Point, 4> TILE_ISO_POINTS { {
    { TILE_CENTRE.x, 0 }, //
    { TILE_SIZE.x, TILE_CENTRE.y }, //
    { TILE_CENTRE.x, TILE_SIZE.y }, //
    { 0, TILE_CENTRE.y } //
} };

inline const std::array<Gate, 4> ROAD_GATES {
    [] {
        std::array<Gate, 4> output {};
        for (auto direction : Direction::EachDirectionIn { Direction::TDirection::ALL_CARDINAL_DIRECTIONS }) {
            auto index_position { Direction::index_position(direction) };
            output[index_position] = {
                TILE_CENTRE + (ROAD_MARK_OFFSET * translations[index_position].entry), // entry
                TILE_CENTRE + (ROAD_MARK_OFFSET * translations[index_position].exit) // exit
            };
        }
        return output;
    }()
};
} // namespace constants

#endif