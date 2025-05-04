#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <vector>
#include <array>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

namespace constants
{
    inline constexpr int FPS{60};
    inline constexpr float MILLIS_PER_FRAME{1'000.f / FPS};

    inline constexpr glm::ivec2 TILE_SIZE{132, 66};
    inline constexpr glm::ivec2 TILE_SIZE_HALF{TILE_SIZE / 2};

    inline constexpr int MAP_BORDER_PX{100};
    inline constexpr int MAP_SIZE_N_TILES{10};
    inline constexpr glm::ivec2 MAP_SIZE_PX{TILE_SIZE * MAP_SIZE_N_TILES};

    inline constexpr glm::ivec2 RENDER_SPACE_SIZE_PX{MAP_SIZE_PX + (MAP_BORDER_PX * 2)};
    inline constexpr glm::ivec2 TILEMAP_START{(RENDER_SPACE_SIZE_PX.x / 2) - (TILE_SIZE.x / 2), MAP_BORDER_PX};

    /*
        This offset is being used as a means of accounting for tile "depth" when converting mouse
        pixel coordinate to the grid coordinate.

        The idea is that if every tile is at least MIN_TILE_DEPTH 'deep', then the real tilemap surface
        starts at the 'top' of that depth, and not the bottom of it (as would be default).

        We 'offset' the tilemap start by the minimum tile depth (e.g. shift the tilemap surface up by
        33 pixels) so that we can still understand which tile the mouse coordinate is 'on' while
        accounting for how tall the tiles are.
    */
    inline constexpr int MIN_TILE_DEPTH{33};
    inline constexpr int MAX_TILE_DEPTH{34};
    inline constexpr int GROUND_FLOOR_BUILDING_OFFSET{77};
    inline constexpr glm::ivec2 OFFSET_TILEMAP_START{TILEMAP_START - glm::ivec2{0, MIN_TILE_DEPTH}};

    inline constexpr int CAMERA_BORDER_PX{80};

    const std::string MAP_TILE_PNG_PATH{
        "/home/marv/Documents/Projects/isometric-game/assets/"
        "kenney_isometric-city/Spritesheet/cityTiles_sheet.png"};

    const std::string MAP_ATLAS_PATH{
        "/home/marv/Documents/Projects/isometric-game/assets/"
        "kenney_isometric-city/Spritesheet/cityTiles_sheet.xml"};

    const std::string BUILDING_TILE_PNG_PATH{
        "/home/marv/Documents/Projects/isometric-game/assets/"
        "kenney_isometric-buildings-1/Spritesheet/buildingTiles_sheet.png"};

    const std::string BUILDING_ATLAS_PATH{
        "/home/marv/Documents/Projects/isometric-game/assets/"
        "kenney_isometric-buildings-1/Spritesheet/buildingTiles_sheet.xml"};

    const std::string VEHICLE_TILE_PNG_PATH{
        "/home/marv/Documents/Projects/isometric-game/assets/"
        "kenney_isometric-vehicles-1/Spritesheet/sheet_allCars.png"};

    const std::string VEHICLE_ATLAS_PATH{
        "/home/marv/Documents/Projects/isometric-game/assets/"
        "kenney_isometric-vehicles-1/Spritesheet/sheet_allCars.xml"};

    const std::string MOUSE_MAP_PNG_PATH{
        "/home/marv/Documents/Projects/isometric-game/assets/mousemap.png"};

    enum Directions
    {
        NO_DIRECTION = 0,
        NORTH = 8,
        EAST = 4,
        SOUTH = 2,
        WEST = 1
    };

    inline constexpr std::array<glm::ivec2, 4> VECTORS{
        // glm::ivec2{0, 0},       // 0 - NO_DIRECTION
        glm::ivec2{-1, 0}, // 1 - WEST
        glm::ivec2{0, 1},  // 2 - SOUTH
        glm::ivec2{1, 0},  // 3 - EAST
        glm::ivec2{0, -1}  // 4 - NORTH
    };

    enum PointsOnTile
    {
        TOP_LEFT,
        LEFT_MIDDLE,
        BOTTOM_LEFT,
        TOP_MIDDLE,
        BOTTOM_MIDDLE,
        TOP_RIGHT,
        RIGHT_MIDDLE,
        BOTTOM_RIGHT,
    };

    constexpr std::array<glm::ivec2, 8> POINTS_ON_TILE = []()
    {
        int count{};
        std::array<glm::ivec2, 8> points = {};

        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 3; y++)
            {

                if (x == 1 && y == 1)
                {
                    continue;
                }

                points[count] = glm::ivec2{
                    constants::TILE_SIZE_HALF * glm::ivec2{x, y}};

                count++;
            }
        }
        return points;
    }();

    constexpr std::array<glm::ivec2, 5> TILE_EDGE_POINTS{
        POINTS_ON_TILE[constants::TOP_MIDDLE],
        POINTS_ON_TILE[constants::RIGHT_MIDDLE],
        POINTS_ON_TILE[constants::BOTTOM_MIDDLE],
        POINTS_ON_TILE[constants::LEFT_MIDDLE],
        POINTS_ON_TILE[constants::TOP_MIDDLE]};
}

#endif