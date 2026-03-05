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

    nlohmann::json component_document_array;
    nlohmann::json current_component_document;
    nlohmann::json current_component_array;
    nlohmann::json context;

    void commit_component_document();
    void commit_to_root();

public:
    OutputArchive()
        : component_document_array { nlohmann::json::array() }
        , current_component_array { nlohmann::json::array() }
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
        current_component_array.push_back(component_json);
    }

    template <typename T>
    void save_context_element(const std::string document_key, const T& element)
    {
        context[document_key] = element;
    }

    void to_file(std::string path);
};

class InputArchive {

    uint32_t component_document_index { 0 };
    uint32_t component_index { 0 };

    nlohmann::json root;

    nlohmann::json component_document_array;
    nlohmann::json current_component_document;
    nlohmann::json current_component_array;
    nlohmann::json active_component;
    nlohmann::json context;

    void fetch_component_document();
    void load_next_component_document();

public:
    InputArchive(std::string file_path);

    // ...to load entities
    void operator()(entt::entity&);

    // ...to read the size of the set they are going to load
    void operator()(std::underlying_type_t<entt::entity>&);

    // ...references to the types of component to restore
    template <typename T>
    void operator()(T& component)
    {
        component = active_component["component"].get<T>();
    }

    template <typename T>
    void load_context_element(const std::string document_key, T& element)
    {
        context.at(document_key).get_to(element);
    }
};

#endif
