#pragma once

/* For making toml++ work on Windows */
#include <toml++/toml.hpp>

#include <unordered_map>
#include "FSConfig.hpp"

/*
	This class is supposed to take care of reading files that are supposed to represent the game's
	configuration
*/
namespace sand {
	enum class ConfigFileExtension {
		INVALID,
		TOML
	};


	class FSConfigReader {
	public:
		FSConfigReader() = default;
		FSConfigReader(const std::string& path);
		FSConfig ParseFile() const;
		void SetPath(const std::string& path);
	private:
		static void ParseDefaults(FSConfig& config, const toml::v3::node_view<toml::v3::node>& defaultSection);
		static ParticleData GetParticleData(const toml::v3::table& particle);
	private:
		std::string path;
		// I don't bother making this static as it will only be used inside this class
		std::unordered_map<std::string, ConfigFileExtension> validExtension;
	};
}
