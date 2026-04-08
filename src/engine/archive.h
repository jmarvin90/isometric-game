#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <SDL2/SDL.h>
#include <algorithm>
#include <entt/entt.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include <json_parse.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <queue>
#include <spritesheet.h>
#include <string>
#include <utility>

#include <spdlog/spdlog.h>

/*
    entt snapshots will call functions to provide/request:
        - size of a pool;
        - the next entity in the current pool,
        - the next component in the pool

    The size call comes at the start of a new pool; it can be inferred that
    any previous/pending pools are concluded when a size call is made

    The first pools contain entities only
        (entt::snapshot_loader { registry }.get<entt::entity>(my_archive)

    I'm not sure if entity/component calls are alternated (e.g. ABAB) or one
    after another (AABB).
*/

template <typename EntityContainer, typename ComponentContainer>
struct ComponentPoolDocument {
    std::underlying_type_t<entt::entity> size;
    EntityContainer entities;
    ComponentContainer components;
    ComponentPoolDocument(std::underlying_type_t<entt::entity> size)
        : size { size }
        , entities(nlohmann::json::array())
        , components(nlohmann::json::array())
    {
    }
    ComponentPoolDocument(nlohmann::json pool_json)
        : size { pool_json["size"].get<std::underlying_type_t<entt::entity>>() }
    {
        for (auto entity : pool_json["entities"]) {
            entities.emplace(entity.get<entt::entity>());
        }

        for (auto component : pool_json["components"]) {
            components.emplace(component);
        }
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ComponentPoolDocument, size, entities, components)
};

namespace {
using ImportComponentDocument = ComponentPoolDocument<std::queue<entt::entity>, std::queue<nlohmann::json>>;
using ExportComponentDocument = ComponentPoolDocument<nlohmann::json, nlohmann::json>;
}

// TODO - is this strictly necessary
struct SpriteRecord {
    std::string name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SpriteRecord, name)
};

class OutputArchive {
    nlohmann::json root;
    std::optional<ExportComponentDocument> current_pool;
    nlohmann::json context;

public:
    void commit_pool();
    void commit_to_root();
    void operator()(entt::entity);
    void operator()(std::underlying_type_t<entt::entity>);

    template <typename T>
    void operator()(const T& component)
    {
        current_pool.value().components.push_back(component);
    }

    void operator()(const SpriteComponent& component);

    template <typename T>
    void save_context_element(const std::string document_key, const T& element)
    {
        context[document_key] = element;
    }

    void to_file(std::string path);
};

/*






*/

class InputArchive {

    const SpriteSheet& spritesheet;
    nlohmann::json root;
    std::queue<ImportComponentDocument> component_pools;
    std::optional<ImportComponentDocument> current_pool;
    nlohmann::json context;

    int count { 0 };

public:
    InputArchive(std::string file_path, const SpriteSheet& spritesheet);
    void operator()(entt::entity&);
    void operator()(std::underlying_type_t<entt::entity>&);

    template <typename T>
    void operator()(T& component)
    {
        T _component { current_pool.value().components.front().get<T>() };
        current_pool.value().components.pop();
        component = _component;
    }

    void operator()(SpriteComponent& component);

    template <typename T>
    void load_context_element(const std::string document_key, T& element) const
    {
        context.at(document_key).get_to(element);
    }
};

#endif
