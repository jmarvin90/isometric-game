#include <iostream>
#include <glm/glm.hpp>
#include "engine/game.h"

#include "tilemap.h"

int main()
{
    Game game;

    game.initialise();

    (*game.tilemap)[{8, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*game.tilemap)[{8, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*game.tilemap)[{8, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*game.tilemap)[{8, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*game.tilemap)[{8, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*game.tilemap)[{7, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*game.tilemap)[{7, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*game.tilemap)[{7, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*game.tilemap)[{6, 2}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_053.png"));

    (*game.tilemap)[{6, 2}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_053.png"));

    (*game.tilemap)[{6, 2}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_053.png"));

    (*game.tilemap)[{6, 2}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_053.png"));

    (*game.tilemap)[{6, 2}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_061.png"));

    game.run();
    game.destroy();
}