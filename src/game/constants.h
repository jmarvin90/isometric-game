#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace constants {
    inline constexpr int FPS               {60};
    inline constexpr int MILLIS_PER_FRAME  {1'000/FPS};
    inline constexpr int WINDOW_HEIGHT     {768};
    inline constexpr int WINDOW_WIDTH      {1024};
    inline constexpr int TILE_HEIGHT       {60};
    inline constexpr int TILE_WIDTH        {120};
    inline constexpr int TILE_HEIGHT_HALF  {TILE_HEIGHT / 2};
    inline constexpr int TILE_WIDTH_HALF   {TILE_WIDTH / 2};
    inline constexpr int MAP_SIZE          {5};
    inline constexpr int TILEMAP_X_START   {(WINDOW_WIDTH / 2) - (TILE_WIDTH / 2)};
    inline constexpr int TILEMAP_Y_START   {100};
}

#endif