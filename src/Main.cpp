#include "core/constants.h"

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>

#include "core/Application.h"
#include <filesystem>
#include <stdexcept>

#include "logger/Logger.h"

bool parseConfig(const toml::parse_result &config, SpectrumParams &spectrum, PBR &pbr, OtherOptions &other);

void checkValues(SpectrumParams &spectrum, PBR &pbr, OtherOptions &other);

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
	PBR material;
	OtherOptions otherOptions;

	if (!parseConfig(config, spectrumParams, material, otherOptions))
	{
		return EXIT_FAILURE;
	}
	checkValues(spectrumParams, material, otherOptions);

	try
	{
		Application app("Ocean Simulation", spectrumParams, material, otherOptions);
		app.run();
	}
	catch (const std::runtime_error &ex)
	{
		Logger::get().log(ex.what(), true);
		return EXIT_FAILURE;
	}
	catch (...)
	{
		Logger::get().log("OTHER ERROR DETECTED", true);
		return EXIT_FAILURE;
	}


	Logger::get().log("Exiting the program");

	return EXIT_SUCCESS;
}

bool parseConfig(const toml::parse_result &config, SpectrumParams &spectrum, PBR &pbr, OtherOptions &other)
{
	if (!config["spectrum"].is_table())
	[[unlikely]]
	{
		Logger::get().log("CONFIG: Table 'spectrum' is missing", true);
		return false;
	}
	else
	{
		if (!config.at_path("spectrum.amplitude").is_floating_point())
		{
			Logger::get().log(
				"CONFIG: In table 'spectrum' field 'amplitude' of type float is missing or has wrong type", true);
			return false;
		}
		spectrum.amplitude = config.at_path("spectrum.amplitude").as_floating_point()->get();
		if (!config.at_path("spectrum.length").is_floating_point())
		{
			Logger::get().log("CONFIG: In table 'spectrum' field 'length' of type float is missing or has wrong type",
			                  true);
			return false;
		}
		spectrum.length = config.at_path("spectrum.length").as_floating_point()->get();
		if (!config.at_path("spectrum.choppinessScale").is_array())
		{
			Logger::get().log(
				"CONFIG: In table 'spectrum' field 'choppinessScale' of type array is missing or has wrong type",
				true);
			return false;
		}
		const auto choppinessScale = config.at_path("spectrum.choppinessScale").as_array();
		if (choppinessScale->as_array()->size() != 2)
		{
			Logger::get().log("CONFIG: In table 'spectrum' field 'choppinessScale' doesn't have two elements", true);
			return false;
		}
		if (!choppinessScale->at(0).is_floating_point() || !choppinessScale->at(1).is_floating_point())
		{
			Logger::get().log("CONFIG: In table 'spectrum' field 'choppinessScale' element(s) type is not float", true);
			return false;
		}
		spectrum.choppinessScale = glm::vec2(
			choppinessScale->at(0).as_floating_point()->get(),
			choppinessScale->at(1).as_floating_point()->get());
		if (!config.at_path("spectrum.displacementScale").is_floating_point())
		{
			Logger::get().log(
				"CONFIG:In table 'spectrum' field 'displacementScale' of type float is missing or has wrong type",
				true);
			return false;
		}
		spectrum.displacementScale = config.at_path("spectrum.displacementScale").as_floating_point()->get();
		if (!config.at_path("spectrum.suppressorFactor").is_floating_point())
		{
			Logger::get().log(
				"CONFIG: In table 'spectrum' field 'suppressorFactor' of type float is missing or has wrong type",
				true);
			return false;
		}
		spectrum.suppressorFactor = config.at_path("spectrum.suppressorFactor").as_floating_point()->get();
		if (!config.at_path("spectrum.windDirection").is_array())
		{
			Logger::get().log(
				"CONFIG: In table 'spectrum' field 'windDirection' of type array is missing or has wrong type",
				true);
			return false;
		}
		const auto windDirection = config.at_path("spectrum.windDirection").as_array();
		if (windDirection->as_array()->size() != 2)
		{
			Logger::get().log("CONFIG: In table 'spectrum' field 'windDirection' doesn't have two elements", true);
			return false;
		}
		if (!windDirection->at(0).is_floating_point() || !windDirection->at(1).is_floating_point())
		{
			Logger::get().log("CONFIG: In table 'spectrum' field 'windDirection' element(s) type is not float", true);
			return false;
		}
		spectrum.windDirection = glm::vec2(windDirection->at(0).as_floating_point()->get(),
		                                   windDirection->at(0).as_floating_point()->get());
		if (!config.at_path("spectrum.windSpeed").is_floating_point())
		{
			Logger::get().log(
				"CONFIG: In table 'windSpeed' field 'windSpeed' of type float is missing or has wrong type",
				true);
			return false;
		}
		spectrum.windSpeed = config.at_path("spectrum.windSpeed").as_floating_point()->get();
	}

	if (!config["pbr"].is_table())
	[[unlikely]]
	{
		Logger::get().log("CONFIG: Table 'pbr' is missing", true);
		return false;
	}
	else
	{
		if (!config.at_path("pbr.roughness").is_floating_point())
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'roughness' of type float is missing or has wrong type",
			                  true);
			return false;
		}
		pbr.roughness = config.at_path("pbr.roughness").as_floating_point()->get();
		if (!config.at_path("pbr.bubbleColor").is_array())
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'bubbleColor' of type array is missing or has wrong type",
			                  true);
			return false;
		}
		const auto bubbleColor = config.at_path("pbr.bubbleColor").as_array();
		if (bubbleColor->as_array()->size() != 3)
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'bubbleColor' doesn't have three elements", true);
			return false;
		}
		if (!bubbleColor->at(0).is_floating_point() || !bubbleColor->at(1).is_floating_point() || !bubbleColor->at(2).
		    is_floating_point())
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'bubbleColor' element(s) type is not float", true);
			return false;
		}
		pbr.bubbleColor = glm::vec3(bubbleColor->at(0).as_floating_point()->get(),
		                            bubbleColor->at(1).as_floating_point()->get(),
		                            bubbleColor->at(2).as_floating_point()->get());
		if (!config.at_path("pbr.bubbleDensity").is_floating_point())
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'bubbleDensity' of type float is missing or has wrong type",
			                  true);
			return false;
		}
		pbr.bubbleDensity = config.at_path("pbr.bubbleDensity").as_floating_point()->get();
		if (!config.at_path("pbr.waveHeight").is_floating_point())
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'waveHeight' of type float is missing or has wrong type",
			                  true);
			return false;
		}
		pbr.waveHeight = config.at_path("pbr.waveHeight").as_floating_point()->get();
		if (!config.at_path("pbr.scatterColor").is_array())
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'scatterColor' of type array is missing or has wrong type",
			                  true);
			return false;
		}
		const auto scatterColor = config.at_path("pbr.scatterColor").as_array();
		if (scatterColor->as_array()->size() != 3)
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'scatterColor' doesn't have three elements", true);
			return false;
		}
		if (!scatterColor->at(0).is_floating_point() || !scatterColor->at(1).is_floating_point() || !scatterColor->at(2)
		    .is_floating_point())
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'scatterColor' element(s) type is not float", true);
			return false;
		}
		pbr.scatterColor = glm::vec3(scatterColor->at(0).as_floating_point()->get(),
		                             scatterColor->at(1).as_floating_point()->get(),
		                             scatterColor->at(2).as_floating_point()->get());
		if (!config.at_path("pbr.scatterStrength").is_floating_point())
		{
			Logger::get().log(
				"CONFIG: In table 'pbr' field 'scatterStrength' of type float is missing or has wrong type",
				true);
			return false;
		}
		pbr.scatterStrength = config.at_path("pbr.scatterStrength").as_floating_point()->get();
		if (!config.at_path("pbr.sunDirection").is_array())
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'sunDirection' of type array is missing or has wrong type",
			                  true);
			return false;
		}
		const auto sunDirection = config.at_path("pbr.sunDirection").as_array();
		if (sunDirection->as_array()->size() != 3)
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'sunDirection' doesn't have three elements", true);
			return false;
		}
		if (!sunDirection->at(0).is_floating_point() || !sunDirection->at(1).is_floating_point() || !sunDirection->at(2)
		    .is_floating_point())
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'sunDirection' element(s) type is not float", true);
			return false;
		}
		pbr.sunDirection = glm::vec3(sunDirection->at(0).as_floating_point()->get(),
		                             sunDirection->at(1).as_floating_point()->get(),
		                             sunDirection->at(2).as_floating_point()->get());
		if (!config.at_path("pbr.sunColor").is_array())
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'sunColor' of type array is missing or has wrong type",
			                  true);
			return false;
		}
		const auto sunColor = config.at_path("pbr.sunColor").as_array();
		if (sunColor->as_array()->size() != 3)
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'sunColor' doesn't have three elements", true);
			return false;
		}
		if (!sunColor->at(0).is_floating_point() || !sunColor->at(1).is_floating_point() || !sunColor->at(2).
		    is_floating_point())
		{
			Logger::get().log("CONFIG: In table 'pbr' field 'scatterColor' element(s) type is not float", true);
			return false;
		}
		pbr.sunColor = glm::vec3(sunColor->at(0).as_floating_point()->get(),
		                         sunColor->at(1).as_floating_point()->get(),
		                         sunColor->at(2).as_floating_point()->get());
		if (!config.at_path("pbr.envLightStrength").is_floating_point())
		{
			Logger::get().log(
				"CONFIG: In table 'pbr' field 'evnLightStrength' of type float is missing or has wrong type",
				true);
			return false;
		}
		pbr.envLightStrength = config.at_path("pbr.envLightStrength").as_floating_point()->get();
		if (!config.at_path("pbr.scatterShadowStrength").is_floating_point())
		{
			Logger::get().log(
				"CONFIG: In table 'pbr' field 'scatterShadowStrength' of type float is missing or has wrong type",
				true);
			return false;
		}
		pbr.scatterShadowStrength = config.at_path("pbr.scatterShadowStrength").as_floating_point()->get();
		if (!config.at_path("pbr.wavePeakScatterStrength").is_floating_point())
		{
			Logger::get().log(
				"CONFIG: In table 'pbr' field 'wavePeakScatterStrength' of type float is missing or has wrong type",
				true);
			return false;
		}
		pbr.wavePeakScatterStrength = config.at_path("pbr.wavePeakScatterStrength").as_floating_point()->get();
	}


	if (!config["other"].is_table())
	[[unlikely]]
	{
		Logger::get().log("CONFIG: Table 'other' is missing", true);
		return false;
	}
	else
	{
		if (!config.at_path("other.timeSpeed").is_floating_point())
		{
			Logger::get().log("CONFIG: In table 'other' field 'timeSpeed' of type float is missing or has wrong type",
			                  true);
			return false;
		}
		other.speed = config.at_path("other.timeSpeed").as_floating_point()->get();
		if (!config.at_path("other.tiling").is_integer())
		{
			Logger::get().log("CONFIG: In table 'other' field 'tiling' of type int is missing or has wrong type",
			                  true);
			return false;
		}
		other.tiling = config.at_path("other.tiling").as_integer()->get();
	}

	return true;
}

void checkValues(SpectrumParams &spectrum, PBR &pbr, OtherOptions &other)
{
	auto constrainRange = [](auto value, auto min, auto max)
	{
		return (value < min) ? min : (value > max) ? max : value;
	};
	spectrum.amplitude = constrainRange(spectrum.amplitude, constants::minAmplitude, constants::maxAmplitude);
	spectrum.length = constrainRange(spectrum.length, constants::minLength, constants::maxLength);
	spectrum.choppinessScale.x = constrainRange(spectrum.choppinessScale.x, constants::minChoppinessScale,
	                                            constants::maxChoppinessScale);
	spectrum.choppinessScale.y = constrainRange(spectrum.choppinessScale.y, constants::minChoppinessScale,
	                                            constants::maxChoppinessScale);
	spectrum.displacementScale = constrainRange(spectrum.displacementScale, constants::minDisplacementScale,
	                                            constants::maxDisplacementScale);
	spectrum.suppressorFactor = constrainRange(spectrum.suppressorFactor, constants::minSuppressorFactor,
	                                           constants::maxSuppressorFactor);
	spectrum.windDirection.x = constrainRange(spectrum.windDirection.x, constants::minWindDirection,
	                                          constants::maxWindDirection);
	spectrum.windDirection.y = constrainRange(spectrum.windDirection.y, constants::minWindDirection,
	                                          constants::maxWindDirection);
	spectrum.windSpeed = constrainRange(spectrum.windSpeed, constants::minWindSpeed, constants::maxWindSpeed);

	pbr.roughness = constrainRange(pbr.roughness, constants::minRoughness, constants::maxRoughness);
	pbr.bubbleColor.x = constrainRange(pbr.bubbleColor.x, 0.0f, 1.0f);
	pbr.bubbleColor.y = constrainRange(pbr.bubbleColor.y, 0.0f, 1.0f);
	pbr.bubbleColor.z = constrainRange(pbr.bubbleColor.z, 0.0f, 1.0f);
	pbr.bubbleDensity = constrainRange(pbr.bubbleDensity, constants::minBubbleDensity, constants::maxBubbleDensity);
	pbr.scatterColor.x = constrainRange(pbr.scatterColor.x, 0.0f, 1.0f);
	pbr.scatterColor.y = constrainRange(pbr.scatterColor.y, 0.0f, 1.0f);
	pbr.scatterColor.z = constrainRange(pbr.scatterColor.z, 0.0f, 1.0f);
	pbr.scatterStrength = constrainRange(pbr.scatterStrength, constants::minScatterStrength,
	                                     constants::maxScatterStrength);
	pbr.sunColor.x = constrainRange(pbr.sunColor.x, 0.0f, 1.0f);
	pbr.sunColor.y = constrainRange(pbr.sunColor.y, 0.0f, 1.0f);
	pbr.sunColor.z = constrainRange(pbr.sunColor.z, 0.0f, 1.0f);
	pbr.waveHeight = constrainRange(pbr.waveHeight, constants::minWaveHeight, constants::maxWaveHeight);
	pbr.envLightStrength = constrainRange(pbr.envLightStrength, constants::minEnvironmentLightStrength,
	                                      constants::maxEnvironmentLightStrength);
	pbr.scatterShadowStrength = constrainRange(pbr.scatterShadowStrength, constants::minScatterShadowStrength,
	                                           constants::maxScatterStrength);
	pbr.wavePeakScatterStrength = constrainRange(pbr.wavePeakScatterStrength, constants::minWavePeakScatterStrength,
	                                             constants::maxWavePeakScatterStrength);

	other.speed = constrainRange(other.speed, constants::minSpeed, constants::maxSpeed);
}
