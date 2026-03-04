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

/*
    Place the current component array into its own component document
    (also containing size), then push that document into the component pool array
*/

void OutputArchive::commit_to_root()
{
    commit_component_document();
    root["component_pools"] = component_pool_array;
    root["context"] = context;
}

void OutputArchive::commit_component_document()
{
    current_component_document["components"] = current_component_array;
    component_pool_array.push_back(current_component_document);
    current_component_array = nlohmann::json::array();
    current_component_document.clear();
}

void OutputArchive::operator()([[maybe_unused]] entt::entity entity)
{
    current_entity = uint32_t(entity);
}

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

bool InputArchive::fetch_component_pool()
{
    if (root_index > root.size())
        return false;

    current_component_pool = root[root_index++];
    return true;
}

void InputArchive::load_next_component_pool()
{
    component_index = 0;
    fetch_component_pool();

    // while (
    //     !current_component_pool.contains("components")
    //     || !current_component_pool.contains("size")
    // ) {
    //     if (!fetch_component_pool())
    //         return;
    // }
}

void InputArchive::operator()(std::underlying_type_t<entt::entity>& size)
{
    load_next_component_pool();
    if (!current_component_pool.contains("size"))
        return;
    int _size { current_component_pool.at("size").get<int>() };
    size = (std::underlying_type_t<entt::entity>)_size;
}

void InputArchive::operator()(entt::entity& entity)
{
    if (current_component_pool["components"].empty()) {
        return;
    }

    current_component = current_component_pool["components"][component_index++];
    int int_entity { current_component["entity_id"] };
    entity = entt::entity(int_entity);
}
