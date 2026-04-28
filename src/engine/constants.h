#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL2/SDL.h>
#include <array>
#include <glm/glm.hpp>
#include <string>

// TODO - probably move gates / translations
namespace {
struct Gate {
    glm::ivec2 entry;
    glm::ivec2 exit;
};

// TODO - investigate whether this is correct - comments or otherwise
inline constexpr std::array<Gate, 4> translations {
    {
        { { 3, -1 }, { 1, -3 } }, // North
        { { -1, -3 }, { -3, -1 } }, // East
        { { -3, 1 }, { -1, 3 } }, // South
        { { 1, 3 }, { 3, 1 } } // West
    }
};
}

namespace constants {
inline constexpr glm::ivec2 TILE_SIZE { 256, 128 };
inline constexpr glm::ivec2 TILE_CENTRE { TILE_SIZE / 2 };
inline constexpr glm::ivec2 CAMERA_SCROLL_SPEED { 18, 9 };
inline constexpr int FPS { 60 };
inline constexpr float MILLIS_PER_FRAME { 1'000.f / FPS };
inline constexpr glm::ivec2 SCENE_BORDER_PX { 150, 150 };
inline constexpr int ROAD_WIDTH_PX { 68 };
inline constexpr glm::ivec2 ROAD_MARK_OFFSET { ROAD_WIDTH_PX / 4, ROAD_WIDTH_PX / 8 };
const std::string spritesheet { "assets/spritesheet_scaled.png" };
const std::string SAVE_FILE_PATH { "save.json" };

inline constexpr std::array<SDL_Point, 4> TILE_ISO_POINTS { {
    { TILE_CENTRE.x, 0 }, //
    { TILE_SIZE.x, TILE_CENTRE.y }, //
    { TILE_CENTRE.x, TILE_SIZE.y }, //
    { 0, TILE_CENTRE.y } //
} };

inline const std::array<Gate, 4> ROAD_GATES {
    [] {
        std::array<Gate, 4> output {};
        for (int index = 0; index < 4; index++) {
            output[index] = {
                TILE_CENTRE + (ROAD_MARK_OFFSET * translations[index].entry), // entry
                TILE_CENTRE + (ROAD_MARK_OFFSET * translations[index].exit) // exit
            };
        }
        return output;
    }()
};
} // namespace constants

#endif