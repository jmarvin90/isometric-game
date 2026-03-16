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
    commit_component_document();
    root["component_pools"] = component_document_array;
    root["context"] = context;
}

/*
    Place the current component array into its own component document
    (also containing size), then push that document into the component pool array
*/
void OutputArchive::commit_component_document()
{
    current_component_document["components"] = current_component_array;
    component_document_array.push_back(current_component_document);
    current_component_array = nlohmann::json::array();
    current_component_document.clear();
}

void OutputArchive::operator()([[maybe_unused]] entt::entity entity)
{
    current_entity = uint32_t(entity);
}

// Effectively signals the start of a new component document -
// meaning we should commit the "old" one and start a new
void OutputArchive::operator()(std::underlying_type_t<entt::entity> size)
{
    if (!current_component_array.empty()) {
        commit_component_document();
    }

    current_component_document["size"] = size;
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
    component_document_array = root["component_pools"];
    context = root["context"];
}

void InputArchive::fetch_component_document()
{
    if (component_document_index > component_document_array.size())
        return;

    current_component_document = component_document_array[component_document_index++];

    if (current_component_document.contains("components")) {
        current_component_array = current_component_document["components"];
    }
}

void InputArchive::load_next_component_document()
{
    component_index = 0;
    fetch_component_document();

    // while (
    //     !current_component_pool.contains("components")
    //     || !current_component_pool.contains("size")
    // ) {
    //     if (!fetch_component_pool())
    //         return;
    // }
}

// Effectively signals the start of a new component document -
// meaning we should forget the "old" one and load a new
// (via fetch_component_document)
void InputArchive::operator()(std::underlying_type_t<entt::entity>& size)
{
    component_index = 0;
    load_next_component_document();
    if (!current_component_document.contains("size"))
        return;
    int _size { current_component_document.at("size").get<int>() };
    size = (std::underlying_type_t<entt::entity>)_size;
}

void InputArchive::operator()(entt::entity& entity)
{
    if (
        current_component_document["components"].empty()
        || current_component_document["components"][0].empty()
    ) {
        return;
    }

    active_component = current_component_array[component_index++];
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
    current_component_array.push_back(component_json);
}
