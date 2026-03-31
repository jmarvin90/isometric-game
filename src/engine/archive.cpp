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
    commit_component_pool();
    root["component_pools"] = component_pools_array;
    root["context"] = context;
}

/*
    Place the current set of entity:component pairs into its own component pool
    (also containing size); push that pool into the component pool array
*/
void OutputArchive::commit_component_pool()
{
    current_component_pool["entity_component_pairs"] = components;
    component_pools_array.push_back(current_component_pool);
    components = nlohmann::json::array();
    current_component_pool.clear();
}

void OutputArchive::operator()(entt::entity entity)
{
    current_entity = uint32_t(entity);
}

// Signals the start of a new component pool - commit the "old" one and start a new
void OutputArchive::operator()(std::underlying_type_t<entt::entity> size)
{
    if (!components.empty()) {
        commit_component_pool();
    }

    current_component_pool["size"] = size;
}

void OutputArchive::to_file(std::string path)
{
    commit_to_root();
    std::ofstream file { path };
    file << root.dump();
}

InputArchive::InputArchive(std::string file_path, const SpriteSheet& spritesheet)
    : spritesheet { spritesheet }
{
    std::ifstream ifs(file_path);
    root = nlohmann::json::parse(ifs);
    component_pools_array = root["component_pools"];
    context = root["context"];
}

void InputArchive::fetch_component_document()
{
    if (component_document_index > component_pools_array.size())
        return;

    current_component_pool = component_pools_array[component_document_index++];

    if (current_component_pool.contains("entity_component_pairs")) {
        components = current_component_pool["entity_component_pairs"];
    }
}

void InputArchive::load_next_component_document()
{
    component_index = 0;
    fetch_component_document();

    // while (
    //     !current_component_pool.contains("entity_component_pairs")
    //     || !current_component_pool.contains("size")
    // ) {
    //     if (!fetch_component_pool())
    //         return;
    // }
}

// Effectively signals the start of a new component pool -
// meaning we should forget the "old" one and load a new
// (via fetch_component_document)
void InputArchive::operator()(std::underlying_type_t<entt::entity>& size)
{
    component_index = 0;
    load_next_component_document();
    if (!current_component_pool.contains("size"))
        return;
    int _size { current_component_pool.at("size").get<int>() };
    size = (std::underlying_type_t<entt::entity>)_size;
}

void InputArchive::operator()(entt::entity& entity)
{
    if (
        current_component_pool["entity_component_pairs"].empty()
        || current_component_pool["entity_component_pairs"][0].empty()
    ) {
        return;
    }

    active_component = components[component_index++];
    int int_entity { active_component["entity_id"] };
    entity = entt::entity(int_entity);
}

void InputArchive::operator()(SpriteComponent& component)
{
    std::string sprite_name { active_component["component"]["name"].get<std::string>() };
    component.sprite_definition = &spritesheet.sprites.at(sprite_name);
}

void OutputArchive::operator()(const SpriteComponent& component)
{
    ComponentPair<SpriteRecord> my_pair { current_entity, SpriteRecord { component.sprite_definition->name } };
    nlohmann::json component_json = my_pair;
    components.push_back(component_json);
}