#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include <json_parse.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

template <typename T>
struct ComponentPair {
    uint32_t entity_id;
    T component;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ComponentPair, entity_id, component)
};

class OutputArchive {

    nlohmann::json root;
    uint32_t current_entity;
    nlohmann::json current_component;
    nlohmann::json current_component_pool;
    void commit();

public:
    OutputArchive()
        : root { nlohmann::json::array() }
        , current_component_pool { nlohmann::json::array() }
    {
    }

    ~OutputArchive()
    {
    }

    // ...to store entities
    void operator()(entt::entity);

    // ...to store aside the size of the set they are going to store
    void operator()(std::underlying_type_t<entt::entity>);

    // ...the types of component to serialize
    template <typename T>
    void operator()(const T& component)
    {
        ComponentPair<T> my_pair { current_entity, component };
        nlohmann::json component_json = my_pair;
        current_component_pool.push_back(component_json);
    }

    void context_vars(entt::registry& registry);
    void to_file(std::string path);
};

class InputArchive {

    uint32_t root_index { 0 };
    uint32_t component_index { 0 };

    nlohmann::json root;
    nlohmann::json current_component_pool;
    nlohmann::json current_component;

    bool fetch_component_pool();
    void load_next_component_pool();

public:
    static InputArchive from_file(std::string file_path);

    InputArchive(std::string file_path)
    // : root { nlohmann::json::parse(json_string) }
    {
        std::ifstream ifs(file_path);
        root = nlohmann::json::parse(ifs);
    }

    // ...to load entities
    void operator()(entt::entity&);

    // ...to read the size of the set they are going to load
    void operator()(std::underlying_type_t<entt::entity>&);

    // ...references to the types of component to restore
    template <typename T>
    void operator()(T& component)
    {
        component = current_component["component"].get<T>();
    }
};

#endif
