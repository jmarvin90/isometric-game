#include <iostream>
#include <glm/glm.hpp>
#include "engine/constants.h"
#include "engine/game.h"
#include "engine/components/transform.h"
#include "engine/components/sprite.h"
#include "engine/components/rigid_body.h"

int main() {
    Game game;

    std::vector<std::string> tile_paths {
        "./assets/road.png",                        // 0
        "./assets/green.png",                       // 1
        "./assets/blue.png",                        // 2
        "./assets/pink.png",                        // 3
        "./assets/tallest_tile.png",                // 4
        "./assets/tall_tile.png",                   // 5
        "./assets/taller_tile.png",                 // 6
        "./assets/BLBR.png",                        // 7
        "./assets/BLTL.png",                        // 8
        "./assets/BLTR.png",                        // 9
        "./assets/BRTR.png",                        // 10
        "./assets/TLBR.png",                        // 11
        "./assets/TLTR.png",                        // 12
        "./assets/BLBRTR.png",                      // 13
        "./assets/moveable_sprite_test.png",        // 14
        "./assets/moveable_sprite_tall_test.png",   // 15
    };

    game.initialise(tile_paths);

    for (int n=0; n<3; n++) {
        entt::entity entity_1 {game.get_tilemap().at(n, 3)};
        entt::entity entity_2 {game.get_tilemap().at(n, 1)};

        game.add_component<VerticalSprite>(entity_1, game.fetch_texture(n+4));
        game.add_component<VerticalSprite>(entity_2, game.fetch_texture(n+4));
    }


    game.run();
    game.destroy();
}