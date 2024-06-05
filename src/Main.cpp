#include "core/constants.h"

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>

#include "core/Application.h"
#include <filesystem>

#include "logger/logger.h"
#include "graphics/shaderException.h"

int main()
{
	if (!std::filesystem::exists(constants::config))
	[[unlikely]]
	{
		Logger::get().log("Config file doesn't exist", true);
		return EXIT_FAILURE;
	}

	toml::parse_result config = toml::parse_file(constants::config);
	if (!config)
	{
		std::stringstream ss;
		ss << "Parsing config failed: " << config.error();
		Logger::get().log(ss.str(), true);
		return EXIT_FAILURE;
	}

	SpectrumParams spectrumParams;
	spectrumParams.N = constants::N;
	if (config["spectrum"].is_table())
	{
		spectrumParams.amplitude = config.at_path("spectrum.amplitude").as_floating_point()->get();
		spectrumParams.length = config.at_path("spectrum.length").as_floating_point()->get();
		spectrumParams.choppinessScale = glm::vec2(
			config.at_path("spectrum.choppinessScale[0]").as_floating_point()->get(),
			config.at_path("spectrum.choppinessScale[1]").as_floating_point()->get());
		spectrumParams.displacementScale = config.at_path("spectrum.displacementScale").as_floating_point()->get();
		spectrumParams.suppressorFactor = config.at_path("spectrum.suppressorFactor").as_floating_point()->get();
		spectrumParams.windDirection = glm::vec2(config.at_path("spectrum.windDirection[0]").as_floating_point()->get(),
		                                         config.at_path(
			                                         "spectrum.windDirection[1]").as_floating_point()->get());
		spectrumParams.windSpeed = config.at_path("spectrum.windSpeed").as_floating_point()->get();
	}

	PBR material;
	if (config["pbr"].is_table())
	{
		material.roughness = config.at_path("pbr.roughness").as_floating_point()->get();
		material.bubbleColor = glm::vec3(config.at_path("pbr.bubbleColor[0]").as_floating_point()->get(),
		                                 config.at_path("pbr.bubbleColor[1]").as_floating_point()->get(),
		                                 config.at_path("pbr.bubbleColor[2]").as_floating_point()->get());
		material.bubbleDensity = config.at_path("pbr.bubbleDensity").as_floating_point()->get();
		material.heightWave = config.at_path("pbr.heightWave").as_floating_point()->get();
		material.scatterColor = glm::vec3(config.at_path("pbr.scatterColor[0]").as_floating_point()->get(),
		                                  config.at_path("pbr.scatterColor[1]").as_floating_point()->get(),
		                                  config.at_path("pbr.scatterColor[2]").as_floating_point()->get());
		material.scatterStrength = config.at_path("pbr.scatterStrength").as_floating_point()->get();
		material.sunDirection = glm::vec3(config.at_path("pbr.sunDirection[0]").as_floating_point()->get(),
		                                  config.at_path("pbr.sunDirection[1]").as_floating_point()->get(),
		                                  config.at_path("pbr.sunDirection[2]").as_floating_point()->get());
		material.sunColor = glm::vec3(config.at_path("pbr.sunColor[0]").as_floating_point()->get(),
		                              config.at_path("pbr.sunColor[1]").as_floating_point()->get(),
		                              config.at_path("pbr.sunColor[2]").as_floating_point()->get());
		material.envLightStrength = config.at_path("pbr.envLightStrength").as_floating_point()->get();
		material.scatterShadowStrength = config.at_path("pbr.scatterShadowStrength").as_floating_point()->get();
		material.wavePeakScatterStrength = config.at_path("pbr.wavePeakScatterStrength").as_floating_point()->get();
	}

	OtherOptions otherOptions;
	if (config["other"].is_table())
	{
		otherOptions.speed = config.at_path("other.timeSpeed").as_floating_point()->get();
		otherOptions.tiling = config.at_path("other.tiling").as_integer()->get();
	}

	Application app("Ocean Simulation", spectrumParams, material, otherOptions);

	try
	{
		app.run();
	}
	catch (const gfx::ShaderException &ex)
	{
		Logger::get().log(ex.what(), true);
		return EXIT_FAILURE;
	}

	Logger::get().log("Exiting the program");

	return EXIT_SUCCESS;
}
