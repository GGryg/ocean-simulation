#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <numbers>

namespace constants
{
    inline constexpr float gravity = 9.81f;
    inline constexpr float pi2 = std::numbers::pi_v<float> * 2;

    inline constexpr int gridSize = 64;
    inline constexpr int gridUVSize = 32;
}

#endif