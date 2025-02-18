#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <vector>
#include <array>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

namespace constants {
    inline constexpr int FPS                            {60};
    inline constexpr float MILLIS_PER_FRAME             {1'000.f/FPS};

    inline constexpr glm::ivec2 TILE_SIZE               {132, 66};
    inline constexpr glm::ivec2 TILE_SIZE_HALF          {TILE_SIZE / 2};
    
    inline constexpr int MAP_BORDER_PX                  {100};
    inline constexpr int MAP_SIZE_N_TILES               {20};
    inline constexpr glm::ivec2 MAP_SIZE_PX             {TILE_SIZE * MAP_SIZE_N_TILES};

    inline constexpr glm::ivec2 RENDER_SPACE_SIZE_PX    {MAP_SIZE_PX + (MAP_BORDER_PX * 2)};
    inline constexpr glm::ivec2 TILEMAP_START           {(RENDER_SPACE_SIZE_PX.x / 2) - (TILE_SIZE.x / 2), MAP_BORDER_PX};

    /*
        This offset is being used as a means of accounting for tile "depth" when converting mouse
        pixel coordinate to the grid coordinate. 

        The idea is that if every tile is at least MIN_TILE_DEPTH 'deep', then the real tilemap surface
        starts at the 'top' of that depth, and not the bottom of it (as would be default).

        We 'offset' the tilemap start by the minimum tile depth (e.g. shift the tilemap surface up by
        33 pixels) so that we can still understand which tile the mouse coordinate is 'on' while
        accounting for how tall the tiles are.
    */
    inline constexpr int MIN_TILE_DEPTH                 {33};
    inline constexpr glm::ivec2 OFFSET_TILEMAP_START    {TILEMAP_START - glm::ivec2{0, MIN_TILE_DEPTH}};

    inline constexpr int CAMERA_BORDER_PX               {80};

    const std::string map_tile_png_path {
        "/home/marv/Documents/Projects/isometric-game/assets/"
        "kenney_isometric-city/Spritesheet/cityTiles_sheet.png"
    };

    const std::string map_atlas_path {
        "/home/marv/Documents/Projects/isometric-game/assets/"
        "kenney_isometric-city/Spritesheet/cityTiles_sheet.xml"
    };

    const std::string building_tile_png_path {
        "/home/marv/Documents/Projects/isometric-game/assets/"
        "kenney_isometric-buildings-1/Spritesheet/buildingTiles_sheet.png"
    };

    const std::string building_atlas_path {
        "/home/marv/Documents/Projects/isometric-game/assets/"
        "kenney_isometric-buildings-1/Spritesheet/buildingTiles_sheet.xml"
    };

    enum Directions {
        NO_DIRECTION,
        LEFT,
        UP,
        RIGHT,
        DOWN
    };

    inline constexpr glm::ivec2 directions[5] {
        glm::ivec2{0, 0},                       // NO_DIRECTION
        glm::ivec2{-1, 0},                      // LEFT
        glm::ivec2{0, -1},                      // UP
        glm::ivec2{1, 0},                       // RIGHT
        glm::ivec2{0, 1}                        // DOWN
    };

    enum PointsOnTile {
        TOP_LEFT,
        LEFT_MIDDLE,
        BOTTOM_LEFT,
        TOP_MIDDLE,
        BOTTOM_MIDDLE,
        TOP_RIGHT,
        RIGHT_MIDDLE,
        BOTTOM_RIGHT,
    };

    constexpr std::array<glm::ivec2, 8> points_on_tile = [](){
        int count{};
        std::array<glm::ivec2, 8> points = {};

        for (int x=0; x<3; x++) {
            for (int y=0; y<3; y++) {

                if (x == 1 && y == 1) {
                    continue;
                }

                points[count] = glm::ivec2{
                    constants::TILE_SIZE_HALF * glm::ivec2{x, y}
                };

                count++;
            }
        }
        return points;
    }();

    constexpr std::array<glm::ivec2, 5> tile_edge_points {
        points_on_tile[constants::TOP_MIDDLE],
        points_on_tile[constants::RIGHT_MIDDLE],
        points_on_tile[constants::BOTTOM_MIDDLE],
        points_on_tile[constants::LEFT_MIDDLE],
        points_on_tile[constants::TOP_MIDDLE]
    };
}

#endif