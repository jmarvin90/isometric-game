#ifndef SPRITE_H
#define SPRITE_H

struct SpriteDefinition;

struct SpriteComponent {
    const SpriteDefinition* sprite_definition;
};

#endif