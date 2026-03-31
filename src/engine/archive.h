#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <SDL2/SDL.h>
#include <algorithm>
#include <entt/entt.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include <json_parse.h>
#include <nlohmann/json.hpp>
#include <spritesheet.h>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

#include <spdlog/spdlog.h>

template <typename T>
struct ComponentPair {
    uint32_t entity_id;
    T component;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ComponentPair, entity_id, component)
};

// TODO - is this strictly necessary
struct SpriteRecord {
    std::string name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SpriteRecord, name)
};

/*

{
    "component_pools": [                    <-- component_pools_array
        {                                   <-- current_component_pool (entity_component_pairs, size)
            "entity_component_pairs": [     <-- components
                {                           <-- ComponentPair (component, entity_id)
                    "component": {          <-- component
                        ...
                    },
                    "entity_id": 0          <-- current_entity
                }
                ...
            ],
            "size": 0                       <-- size
        }
    ]
}
*/

class OutputArchive {

    nlohmann::json root;
    uint32_t current_entity;

    std::vector<std::pair<std::type_index, nlohmann::json>> component_pools;
    nlohmann::json context;

public:
    ~OutputArchive() { }

    void commit_to_root();

    // ...to store entities
    void operator()(entt::entity);

    // ...to store aside the size of the set they are going to store
    void operator()(std::underlying_type_t<entt::entity>);

    // ...the types of component to serialize
    template <typename T>
    void operator()(const T& component)
    {
        auto it = std::find_if(
            component_pools.begin(), component_pools.end(),
            [&](const auto& pair) { return pair.first == std::type_index(typeid(T)); }
        );

        if (it == component_pools.end()) {
            component_pools.emplace_back(std::type_index(typeid(T)), nlohmann::json::array());
        }

        nlohmann::json component_json = ComponentPair<T> { current_entity, component };
        component_pools.back().second.push_back(component_json);
    }

    // Special overload to ensure we save just the sprite ID
    void operator()(const SpriteComponent& component);

    template <typename T>
    void save_context_element(const std::string document_key, const T& element)
    {
        context[document_key] = element;
    }

    void to_file(std::string path);
};

class InputArchive {

    const SpriteSheet& spritesheet;
    nlohmann::json root;
    nlohmann::json component_pools;
    nlohmann::json context;
    nlohmann::json current_component;

    int current_component_pool_n { 0 };
    int current_component_n { 0 };

    void next_component();

public:
    InputArchive(std::string file_path, const SpriteSheet& spritesheet);

    // ...to load entities
    void operator()(entt::entity&);

    // ...to read the size of the component pool they are going to load
    void operator()(std::underlying_type_t<entt::entity>&);

    // ...references to the types of component to restore
    template <typename T>
    void operator()(T& component)
    {
        spdlog::info(current_component.dump());
        component = current_component.get<T>();
    }

    void operator()(SpriteComponent& component);

    template <typename T>
    void load_context_element(const std::string document_key, T& element) const
    {
        context.at(document_key).get_to(element);
    }
};

#endif
