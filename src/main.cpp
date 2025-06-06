#include <iostream>
#include <glm/glm.hpp>
#include "engine/game.h"

#include "tilemap.h"
#include "asset_manager.h"

int main()
{
    Game game;

    game.initialise();

    (*game.tilemap)[{8, 0}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_043.png"));

    (*game.tilemap)[{8, 0}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_043.png"));

    (*game.tilemap)[{8, 0}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_043.png"));

    (*game.tilemap)[{8, 0}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_043.png"));

    (*game.tilemap)[{8, 0}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_043.png"));

    (*game.tilemap)[{7, 0}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_043.png"));

    (*game.tilemap)[{7, 0}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_043.png"));

    (*game.tilemap)[{7, 0}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_043.png"));

    (*game.tilemap)[{6, 2}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_053.png"));

    (*game.tilemap)[{6, 2}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_053.png"));

    (*game.tilemap)[{6, 2}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_053.png"));

    (*game.tilemap)[{6, 2}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_053.png"));

    (*game.tilemap)[{6, 2}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_053.png"));

    (*game.tilemap)[{6, 2}].add_building_level(
        game.asset_manager->get_sprite("buildingTiles_061.png"));

    game.run();
    game.destroy();
}