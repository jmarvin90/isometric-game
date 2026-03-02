#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
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

public:
    OutputArchive()
        : root { nlohmann::json::array() }
        , current_component_pool { nlohmann::json::array() }
    {
    }

    ~OutputArchive()
    {
        spdlog::info(root.dump());
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

    void close()
    {
        root.push_back(current_component_pool);
    }
};

class InputArchive {
public:
    // ...to load entities
    void operator()(entt::entity&);

    // ...to read the size of the set they are going to load
    void operator()(std::underlying_type_t<entt::entity>&);

    // ...references to the types of component to restore
    template <typename T>
    void operator()(T&);
};

#endif
