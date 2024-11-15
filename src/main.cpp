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

    auto entity {game.create_entity()};

    int width_px;
    int height_px;

    SDL_Texture* my_texture {game.fetch_texture(15)};

    SDL_QueryTexture(
        my_texture, NULL, NULL, &width_px, &height_px
    );

    glm::vec2 position {game.get_tilemap().grid_to_pixel(5, 2)};
    position.x += (constants::TILE_WIDTH_HALF - (width_px / 2));
    position.y -= (constants::TILE_HEIGHT_HALF);

    game.add_component<Transform>(entity, position, 0.0);
    game.add_component<VerticalSprite>(entity, height_px, width_px, 15, constants::TILE_HEIGHT-height_px, 0);
    game.add_component<RigidBody>(entity, glm::vec2(-60, -30));


    for (int n=0; n<3; n++) {
        entt::entity entity_1 {game.get_tilemap().at(n, 3)};
        entt::entity entity_2 {game.get_tilemap().at(n, 1)};

        int height_px;
        int width_px;

        SDL_QueryTexture(
            game.fetch_texture(n+4), NULL, NULL, &width_px, &height_px
        );

        game.add_component<VerticalSprite>(
            entity_1,
            height_px,
            width_px,
            n+4,
            constants::TILE_HEIGHT - height_px,
            constants::TILE_WIDTH - width_px
        );

        game.add_component<VerticalSprite>(
            entity_2,
            height_px,
            width_px,
            n+4,
            constants::TILE_HEIGHT - height_px,
            constants::TILE_WIDTH - width_px
        );
    }


    game.run();
    game.destroy();
}