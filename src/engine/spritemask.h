#ifndef SPRITEMASK_H
#define SPRITEMASK_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>

namespace SpriteMask {

std::vector<bool> get_mask(
    const SDL_Surface* surface,
    const SDL_Rect rect,
    [[maybe_unused]] std::string name // helpful in debug, remove later
);

bool pixel_is_opaque(const SDL_Surface* surface, int x, int y);
bool is_highlight_pixel(const SDL_Surface* surface, int x, int y);
void set_outline_texture(SDL_Texture* texture, SDL_Surface* surface);

}

#endif