#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string_view>

namespace constants
{
	inline constexpr int gridSize = 1024;
	inline constexpr int gridSizeP1 = gridSize + 1;
	inline constexpr int gridUVSize = 8;

	inline constexpr int N = 256; // MUST BE POWER OF 2
	inline constexpr std::string_view config = "config.toml";

	inline constexpr int windowWidth = 1600;
	inline constexpr int windowHeight = 900;
}

#endif
