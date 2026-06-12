#ifndef PATHCOMPONENT_H
#define PATHCOMPONENT_H

#include <components/path_component.h>
#include <entt/entt.hpp>
#include <vector>

struct PathComponent {
    std::vector<PathSegment> path;
    int current { 0 };
};

#endif