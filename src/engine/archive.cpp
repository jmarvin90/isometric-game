#include <SDL2/SDL.h>
#include <archive.h>
#include <entt/entt.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include <grid.h>
#include <json_parse.h>
#include <nlohmann/json.hpp>
#include <projection.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>

void OutputArchive::commit_pool()
{
    root["component_pools"].push_back(current_pool.value());
    current_pool.reset();
}

// Commit the component pool array and the context document into the root JSON
void OutputArchive::commit_to_root()
{
}

void OutputArchive::operator()(entt::entity entity)
{
    current_pool.value().entities.push_back(entity);
}

void OutputArchive::operator()([[maybe_unused]] std::underlying_type_t<entt::entity> size)
{
    if (current_pool) {
        commit_pool();
    }

    current_pool.emplace(ComponentPoolDocument { size });
}

void OutputArchive::to_file(std::string path)
{
    if (current_pool.has_value()) {
        commit_pool();
    }
    root["context"] = context;
    std::ofstream file { path };
    file << root.dump();
}

void OutputArchive::operator()(const SpriteComponent& component)
{
    current_pool.value().components.push_back(SpriteRecord { component.sprite_definition->name });
}

/*






*/

InputArchive::InputArchive(std::string file_path, const SpriteSheet& spritesheet)
    : spritesheet { spritesheet }
{
    std::ifstream ifs(file_path);
    root = nlohmann::json::parse(ifs);

    for (auto pool : root["component_pools"]) {
        component_pools.emplace(ComponentPoolDocumentQueue(pool));
    }

    context = root["context"];
}

void InputArchive::operator()([[maybe_unused]] std::underlying_type_t<entt::entity>& size)
{
    if (current_pool)
        current_pool.reset();

    current_pool.emplace(component_pools.front());
    component_pools.pop();
    size = current_pool.value().size;
}

void InputArchive::operator()(entt::entity& entity)
{
    entt::entity current_entity = current_pool.value().entities.front();
    current_pool.value().entities.pop();
    entity = current_entity;
}

void InputArchive::operator()(SpriteComponent& component)
{
    // can't use brace initialisation here
    nlohmann::json _component = current_pool.value().components.front();
    current_pool.value().components.pop();
    std::string sprite_name { _component["name"].get<std::string>() };
    component.sprite_definition = &spritesheet.sprites.at(sprite_name);
}