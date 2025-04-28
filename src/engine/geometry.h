#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cstdint>

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include "constants.h"


class Direction {

    protected:
        uint8_t m_direction;
    
    public:
        Direction(uint8_t direction): m_direction{direction} {};

        operator bool() const {
            return bool(m_direction);
        }

        Direction operator-() const {
            return (m_direction >> 2 | m_direction << 2) & 15;
        }

        uint8_t operator~() const {
            return ~m_direction;
        }

        uint8_t operator&(const Direction& direction) const {
            return uint8_t(m_direction & direction.m_direction);
        }

        uint8_t operator&(const uint8_t mask) const {
            return uint8_t(m_direction & mask);
        }

        uint8_t operator|(const Direction& direction) const {
            return uint8_t(m_direction | direction.m_direction);
        }

        int direction_index() const {
            return __builtin_ctz(m_direction);
        }

        glm::ivec2 as_vector() const {
            glm::ivec2 output{constants::VECTORS.at(direction_index())};
            if (output.x == 0 && output.y == 0) {
                spdlog::info("Si problemas");
            }
            return output;
        }
};

#endif