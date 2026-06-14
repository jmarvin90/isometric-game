#ifndef PATHCOMPONENT_H
#define PATHCOMPONENT_H

#include <components/path_component.h>
#include <directions.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <vector>

struct PathSegment {
    glm::vec2 start;
    glm::vec2 end;
    Direction::TDirection direction;
    PathSegment(glm::vec2 start, glm::vec2 end, Direction::TDirection direction)
        : start { start }
        , end { end }
        , direction { direction }
    {
    }
};

struct PathComponent {
    std::vector<PathSegment> path;
    int current { 0 };
};

#endif