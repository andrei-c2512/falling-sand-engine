#include "falling-sand/FSConfigReader.hpp"
#include <filesystem>
#include <Windows.h>

namespace sand {
	FSConfigReader::FSConfigReader(const std::string& path0) {
		path = path0;

		validExtension.emplace(".toml", ConfigFileExtension::TOML);
	}

	FSConfig FSConfigReader::ParseFile() const {
		FSConfig engineConfig;
		try {
			// I don't bother with string_view optimizations here, as it really isn't important
			if (path.size() == 0) {
				throw std::exception("Please provide a path for the configuration reader");
			}

			if (std::filesystem::is_regular_file(path) == false) {
				throw std::exception("Please provide a path to an actual file");
			}

			std::filesystem::path filePath = path;
			std::string extension = filePath.extension().string();

			if (validExtension.find(extension) == validExtension.end()) {
				throw std::exception(("Unsupported file extension: " + extension).c_str());
			}

			auto configFile = toml::parse_file(path);
			// getting the defaults

			auto defaultsSection = configFile["defaults"];

			ParseDefaults(engineConfig, defaultsSection);


			if (auto particles = configFile["particle"].as_array()) {
				for (auto&& particle : *particles) {
					auto& table = *particle.as_table();
					engineConfig.AddParticleType(GetParticleData(table));
				}
			}
			engineConfig.GenerateParticleMap();
			return engineConfig;
		}
		catch (const toml::v3::ex::parse_error& e) {
			OutputDebugStringA(e.what());
		}
		catch (const std::exception& e) {
			OutputDebugStringA(e.what());
		}
		return engineConfig;
	}


	void FSConfigReader::ParseDefaults(FSConfig& config, const toml::v3::node_view<toml::v3::node>& defaultSection) {
		// game loop
		uint16_t updateLimit = defaultSection["update_limit"].value_or(FSConfig::Defaults::UpdateLimit);
		float gameSpeed = defaultSection["game_speed"].value_or(FSConfig::Defaults::GameSpeed);

		auto defaultColorHex = defaultSection["color"].value<std::string>();
		chili::Color defaultColor;
		if (defaultColorHex.has_value())
			defaultColor = chili::Color(*defaultColorHex);
		else
			defaultColor = FSConfig::Defaults::ParticleColor;


		uint8_t sinkChance = defaultSection["sink_chance"].value_or(FSConfig::Defaults::SinkChance);
		uint8_t spread = defaultSection["spread"].value_or(FSConfig::Defaults::ParticleSpread);
		uint8_t burnChance = defaultSection["burn_chance"].value_or(FSConfig::Defaults::BurnChance);

		// falling-sand settings
	
		// modifying the config class

		config.SetSpeed(gameSpeed)
			.SetUpdateLimit(updateLimit)
			.SetSinkChance(sinkChance)
			.SetParticleSpread(spread)
			.SetBurnChance(burnChance)
			.SetParticleColor(defaultColor);
	}

	ParticleData FSConfigReader::GetParticleData(const toml::v3::table& particle) {
		ParticleData data;

		auto colorArray = particle.get_as<toml::array>("color");
	
		for (auto&& color : *colorArray) {
			if (auto hex = color.value<std::string>()) {
				data.AddColor(chili::Color(*hex));
			}
		}

		uint8_t sinkChance = particle["sink_chance"].value_or(FSConfig::Defaults::SinkChance);
		uint8_t spread = particle["spread"].value_or(FSConfig::Defaults::ParticleSpread);
		uint8_t burnChance = particle["burn_chance"].value_or(FSConfig::Defaults::BurnChance);

		return data;
	}

	void FSConfigReader::SetPath(const std::string& path0) {
		path = path0;
	}
}