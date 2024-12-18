#include <iostream>
#include <glm/glm.hpp>
#include "engine/constants.h"
#include "engine/game.h"
#include "engine/components/transform.h"
#include "engine/components/sprite.h"
#include "engine/components/rigid_body.h"

int main() {
    Game game;

    game.initialise("/home/marv/Documents/Projects/isometric-game/assets/");

    for (int n=0; n<3; n++) {
        std::vector<std::string> building_textures {
            "tall_tile.png", "taller_tile.png", "tallest_tile.png"
        };

        entt::entity entity_1 {game.get_tilemap().at(n, 3)};
        Transform entity_1_transform {game.get_component<Transform>(entity_1)};

        entt::entity entity_2 {game.get_tilemap().at(n, 1)};
        Transform entity_2_transform {game.get_component<Transform>(entity_2)};

        entt::entity new_entity {game.create_entity()};
        game.add_component<Transform>(new_entity, entity_1_transform.position, 1, entity_2_transform.rotation);
        game.add_component<Sprite>(new_entity, game.fetch_texture(building_textures[n]));

        entt::entity new_entity_2 {game.create_entity()};
        game.add_component<Transform>(new_entity_2, entity_2_transform.position, 1, entity_2_transform.rotation);
        game.add_component<Sprite>(new_entity_2, game.fetch_texture(building_textures[n]));
    }


    game.run();
    game.destroy();
}