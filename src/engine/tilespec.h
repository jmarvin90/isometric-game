#ifndef TILESPEC_H
#define TILESPEC_H

#include <glm/glm.hpp>

struct TileSpec {
    const glm::ivec2 size;
    const glm::mat2 matrix;
    const glm::mat2 matrix_inverted;
    TileSpec(const glm::ivec2 size)
    : size{size}
    , matrix {size.x / 2.0f, size.y / 2.0f, -size.x / 2.0f, size.y / 2.0f}
    , matrix_inverted {glm::inverse(matrix)} 
    {}
    ~TileSpec() = default;
    TileSpec(const TileSpec&) = delete;
};

#endif