#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <SDL2/SDL.h>

#include <vector>

struct HighlightComponent {
    SDL_Color colour;
    std::vector<SDL_Point> points;
};

#endif