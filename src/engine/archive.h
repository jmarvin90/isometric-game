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

struct ComponentPoolDocumentQueue {
    std::underlying_type_t<entt::entity> size;
    std::queue<entt::entity> entities;
    std::queue<nlohmann::json> components;
    ComponentPoolDocumentQueue(nlohmann::json pool_json)
        : size { pool_json["size"].get<std::underlying_type_t<entt::entity>>() }
    {
        for (auto entity : pool_json["entities"]) {
            if (entity.is_array() && entity.empty())
                break;
            entities.emplace(entity.get<entt::entity>());
        }

        for (auto component : pool_json["components"]) {
            if (component.is_array() && component.empty())
                break;
            components.emplace(component);
        }
    }
};

struct ComponentPoolDocument {
    std::underlying_type_t<entt::entity> size;
    nlohmann::json entities;
    nlohmann::json components;
    ComponentPoolDocument(std::underlying_type_t<entt::entity> size)
        : size { size }
        , entities { nlohmann::json::array() }
        , components { nlohmann::json::array() }
    {
    }
    ComponentPoolDocument()
    // : entities { nlohmann::json::array() }
    // , components { nlohmann::json::array() }
    {
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ComponentPoolDocument, size, entities, components)
};

// TODO - is this strictly necessary
struct SpriteRecord {
    std::string name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SpriteRecord, name)
};

class OutputArchive {
    nlohmann::json root;
    std::optional<ComponentPoolDocument> current_pool;
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
    std::queue<ComponentPoolDocumentQueue> component_pools;
    std::optional<ComponentPoolDocumentQueue> current_pool;
    nlohmann::json context;

    int count { 0 };

public:
    InputArchive(std::string file_path, const SpriteSheet& spritesheet);
    void operator()(entt::entity&);
    void operator()(std::underlying_type_t<entt::entity>&);

    template <typename T>
    void operator()(T& component)
    {
        spdlog::info(current_pool.value().components.front().dump());
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
