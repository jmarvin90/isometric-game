#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <vector>
#include <SDL2/SDL.h>

struct Highlight {
    SDL_Color colour;
    std::vector<SDL_Point> points;
    Highlight() = default;
    ~Highlight() = default;
};

#endif