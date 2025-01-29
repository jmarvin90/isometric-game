#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace constants {
    inline constexpr int FPS                    {60};
    inline constexpr float MILLIS_PER_FRAME     {1'000.f/FPS};
    inline constexpr int WINDOW_HEIGHT          {768};
    inline constexpr int WINDOW_WIDTH           {1024};

    inline constexpr int TILE_HEIGHT            {60};
    inline constexpr int TILE_WIDTH             {120};
    inline constexpr int TILE_HEIGHT_HALF       {TILE_HEIGHT / 2};
    inline constexpr int TILE_WIDTH_HALF        {TILE_WIDTH / 2};
    
    inline constexpr int MAP_BORDER_PX          {100};
    inline constexpr int MAP_SIZE_N_TILES       {60};
    inline constexpr int MAP_WIDTH_PX           {MAP_SIZE_N_TILES * TILE_WIDTH};
    inline constexpr int MAP_HEIGHT_PX          {MAP_SIZE_N_TILES * TILE_HEIGHT};

    inline constexpr int RENDER_SPACE_WIDTH_PX  {MAP_WIDTH_PX + (MAP_BORDER_PX * 2)};
    inline constexpr int RENDER_SPACE_HEIGHT_PX {MAP_HEIGHT_PX + (MAP_BORDER_PX * 2)};
    
    inline constexpr int TILEMAP_X_START        {(RENDER_SPACE_WIDTH_PX / 2) - (TILE_WIDTH / 2)};
    inline constexpr int TILEMAP_Y_START        {MAP_BORDER_PX};
    inline constexpr int CAMERA_BORDER_PX       {80};
}

#endif