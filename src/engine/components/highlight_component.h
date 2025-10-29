#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <vector>
#include <SDL2/SDL.h>

struct HighlightComponent {
    SDL_Color colour;
    std::vector<SDL_Point> points;
};

struct TileHighlightComponentComponent: public HighlightComponent {};

#endif