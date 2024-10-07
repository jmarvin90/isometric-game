#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

struct Sprite {
    int height_px;
    int width_px;
    int texture_id;
    int vertical_offset_px;
    int horitonzal_offset_px;
};

struct TerrainSprite: public Sprite {   
};

struct VerticalSprite: public Sprite {
};

#endif