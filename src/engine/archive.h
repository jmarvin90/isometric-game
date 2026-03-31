#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include <json_parse.h>
#include <nlohmann/json.hpp>
#include <spritesheet.h>
#include <string>
#include <vector>

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
    std::underlying_type_t<entt::entity> current_pool_size;

    nlohmann::json component_pools_array;
    nlohmann::json current_component_pool;
    nlohmann::json components;
    nlohmann::json context;

    void commit_component_document();
    void commit_to_root();

public:
    OutputArchive()
        : component_pools_array { nlohmann::json::array() }
        , components { nlohmann::json::array() }
    {
    }

    ~OutputArchive() { }

    // ...to store entities
    void operator()(entt::entity);

    // ...to store aside the size of the set they are going to store
    void operator()(std::underlying_type_t<entt::entity>);

    // ...the types of component to serialize
    template <typename T>
    void operator()(const T& component)
    {
        nlohmann::json component_json = ComponentPair<T> { current_entity, component };
        components.push_back(component_json);
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

    uint32_t component_document_index { 0 };
    uint32_t component_index { 0 };

    nlohmann::json root;

    nlohmann::json component_pools_array;
    nlohmann::json current_component_pool;
    nlohmann::json components;
    nlohmann::json active_component;
    nlohmann::json context;

    void fetch_component_document();
    void load_next_component_document();

public:
    InputArchive(std::string file_path, const SpriteSheet& spritesheet);

    // ...to load entities
    void operator()(entt::entity&);

    // ...to read the size of the component pool they are going to load
    void operator()(std::underlying_type_t<entt::entity>&);

    // ...references to the types of component to restore
    template <typename T>
    void operator()(T& component) const
    {
        component = active_component["component"].get<T>();
    }

    void operator()(SpriteComponent& component);

    template <typename T>
    void load_context_element(const std::string document_key, T& element) const
    {
        context.at(document_key).get_to(element);
    }
};

#endif
