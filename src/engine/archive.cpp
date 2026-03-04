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

void OutputArchive::commit()
{
    current_component["components"] = current_component_pool;
    root.push_back(current_component);
    current_component_pool = nlohmann::json::array();
    current_component.clear();
}

void OutputArchive::operator()([[maybe_unused]] entt::entity entity)
{
    current_entity = uint32_t(entity);
}

void OutputArchive::operator()(std::underlying_type_t<entt::entity> size)
{
    if (!current_component.empty()) {
        commit();
    }

    current_component["size"] = size;
}

void OutputArchive::context_vars(entt::registry& registry)
{
    root.push_back(
        { { "tilemap", registry.ctx().get<const Grid<TileMapProjection>>() } }
    );

    root.push_back(
        { { "spatialmap", registry.ctx().get<const Grid<SpatialMapProjection>>() } }
    );
}

void OutputArchive::to_file(std::string path)
{
    commit();
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
