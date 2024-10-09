#ifndef MOUSEMAP_H
#define MOUSEMAP_H

#include <SDL2/SDL.h>
#include <string>

class MouseMap {
    SDL_Surface* mousemap;
    public:
        MouseMap(const std::string mousemap_file_path);
        ~MouseMap();
        SDL_Color get_pixel(const int x, const int y) const;
};

#endif