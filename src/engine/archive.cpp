#include <archive.h>
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

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
