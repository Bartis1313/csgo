#pragma once

#include <filesystem>
#include <vector>
#include <string>

namespace config
{
	struct Localization
	{
		std::filesystem::path path;
		std::string defaultConfigName;
		std::filesystem::path utilityPath;
		std::string defaultLoadName;
	};

	bool save(const std::string& file);
	bool load(const std::string& file);
	bool startLoad();
	bool startSave(const std::string& name);
	void deleteCfg(const std::string& file);
	bool init(const Localization& localization);
	void reload();
	
	// file to load
	[[nodiscard]] std::string getCfgToLoad();
	// get main folder
	[[nodiscard]] std::filesystem::path getHackPath();
	// get DEFAULT name, returns the field m_defaultConfig which never change
	[[nodiscard]] std::string getDefaultConfigName();
	// get all files
	[[nodiscard]] std::vector<std::string> getAllConfigFiles();
	// extra folder for utilities
	[[nodiscard]] std::filesystem::path getExtraLoadPath();
}