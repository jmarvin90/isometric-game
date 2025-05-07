#include <iostream>
#include <glm/glm.hpp>
#include "engine/game.h"

#include "tilemap.h"

int main()
{
    Game game;

    game.initialise();

    TileMap *tilemap{game.get_tilemap()};

    (*tilemap)[{8, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*tilemap)[{8, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*tilemap)[{8, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*tilemap)[{8, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*tilemap)[{8, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*tilemap)[{7, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*tilemap)[{7, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*tilemap)[{7, 0}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_043.png"));

    (*tilemap)[{6, 2}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_053.png"));

    (*tilemap)[{6, 2}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_053.png"));

    (*tilemap)[{6, 2}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_053.png"));

    (*tilemap)[{6, 2}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_053.png"));

    (*tilemap)[{6, 2}].add_building_level(
        game.building_tiles->get_spritesheet_texture(),
        game.building_tiles->get_sprite_rect("buildingTiles_061.png"));

    game.run();
    game.destroy();
}