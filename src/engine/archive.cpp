#include <SDL2/SDL.h>
#include <archive.h>
#include <entt/entt.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include <grid.h>
#include <json_parse.h>
#include <nlohmann/json.hpp>
#include <projection.h>
#include <sstream>
#include <string>

// Commit the component pool array and the context document into the root JSON
void OutputArchive::commit_to_root()
{
    root["component_pools"] = nlohmann::json::array();
    for (auto& [type, pool] : component_pools) {
        root["component_pools"].push_back(pool);
    }
    root["context"] = context;
}

void OutputArchive::operator()(entt::entity entity)
{
    current_entity = uint32_t(entity);
}

void OutputArchive::operator()([[maybe_unused]] std::underlying_type_t<entt::entity> size)
{
}

void OutputArchive::to_file(std::string path)
{
    commit_to_root();
    std::ofstream file { path };
    file << root.dump();
}

void OutputArchive::operator()(const SpriteComponent& component)
{
    auto it = std::find_if(
        component_pools.begin(), component_pools.end(),
        [&](const auto& pair) { return pair.first == std::type_index(typeid(SpriteComponent)); }
    );

    if (it == component_pools.end()) {
        component_pools.emplace_back(std::type_index(typeid(SpriteComponent)), nlohmann::json::array());
    }

    nlohmann::json component_json = ComponentPair<SpriteRecord> { current_entity, component.sprite_definition->name };
    component_pools.back().second.push_back(component_json);
}

/*






*/

InputArchive::InputArchive(std::string file_path, const SpriteSheet& spritesheet)
    : spritesheet { spritesheet }
{
    std::ifstream ifs(file_path);
    root = nlohmann::json::parse(ifs);
    component_pools = root["component_pools"];
    context = root["context"];
    [[maybe_unused]] int pools_size { int(component_pools.size()) };
}

void InputArchive::next_component()
{
    if (current_component_n == int(component_pools[current_component_pool_n].size()) - 1) {
        current_component_n = 0;
        current_component_pool_n++;
    } else {
        current_component_n++;
    }
}

void InputArchive::operator()([[maybe_unused]] std::underlying_type_t<entt::entity>& size)
{
    size = std::underlying_type_t<entt::entity>(component_pools[current_component_pool_n].size());
}

void InputArchive::operator()(entt::entity& entity)
{
    const auto& current_component_pool { component_pools[current_component_pool_n] };
    current_component = current_component_pool[current_component_n]["component"];
    entity = current_component_pool[current_component_n]["entity_id"].get<entt::entity>();
    next_component();
}

void InputArchive::operator()(SpriteComponent& component)
{
    std::string sprite_name { current_component["name"].get<std::string>() };
    component.sprite_definition = &spritesheet.sprites.at(sprite_name);
}