#ifndef GLMJSON_H
#define GLMJSON_H

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace glm {
void to_json(nlohmann::json& j, const glm::ivec2& P);
void to_json(nlohmann::json& j, const glm::vec2& P);

void from_json(const nlohmann::json& j, glm::ivec2& P);
void from_json(const nlohmann::json& j, glm::vec2& P);
}

#endif