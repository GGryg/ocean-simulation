#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <numbers>
#include <string_view>

namespace constants
{
    inline constexpr float gravity = 9.81f;
    inline constexpr float pi2 = std::numbers::pi_v<float> * 2;
    inline constexpr float pi = std::numbers::pi_v<float>;

    inline constexpr int gridSize = 1024;
    inline constexpr int gridSizeP1 = gridSize + 1;
    inline constexpr int gridUVSize = 1;

    inline constexpr int N = 256; // MUST BE POWER OF 2
    inline constexpr std::string_view config = "config.toml";
}

#endif