#ifndef SPRITE_H
#define SPRITE_H

struct Sprite {
    const SDL_Rect source_rect;
    SDL_Texture* texture;
};

#endif