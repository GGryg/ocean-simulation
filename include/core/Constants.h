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

	// for restricting parameters
	inline constexpr float minAmplitude = 0.0f;
	inline constexpr float maxAmplitude = 80.0f;
	inline constexpr float minWindSpeed = 0.0f;
	inline constexpr float maxWindSpeed = 35.0f;
	inline constexpr float minWindDirection = -1.0f;
	inline constexpr float maxWindDirection = 1.0f;
	inline constexpr float minLength = 100.0f;
	inline constexpr float maxLength = 1000.0f;
	inline constexpr float minSuppressorFactor = 0.0f;
	inline constexpr float maxSuppressorFactor = 30.0f;
	inline constexpr float minDisplacementScale = 0.0f;
	inline constexpr float maxDisplacementScale = 2.0f;
	inline constexpr float minChoppinessScale = 0.0f;
	inline constexpr float maxChoppinessScale = 1.5f;
	inline constexpr float minRoughness = 0.01f;
	inline constexpr float maxRoughness = 1.0f;
	inline constexpr float minWaveHeight = 0.0f;
	inline constexpr float maxWaveHeight = 10.0f;
	inline constexpr float minBubbleDensity = 0.0f;
	inline constexpr float maxBubbleDensity = 10.0f;
	inline constexpr float minWavePeakScatterStrength = 0.0f;
	inline constexpr float maxWavePeakScatterStrength = 10.0f;
	inline constexpr float minScatterStrength = 0.0f;
	inline constexpr float maxScatterStrength = 10.0f;
	inline constexpr float minScatterShadowStrength = 0.0f;
	inline constexpr float maxScatterShadowStrength = 10.0f;
	inline constexpr float minEnvironmentLightStrength = 0.0f;
	inline constexpr float maxEnvironmentLightStrength = 10.0f;
	inline constexpr float minSpeed = -10.0f;
	inline constexpr float maxSpeed = 10.0f;
}

#endif
