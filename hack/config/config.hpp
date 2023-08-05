#pragma once

#include <filesystem>
#include <vector>
#include <string>

namespace config
{
	bool save(const std::string& file);
	bool load(const std::string& file);
	bool startLoad();
	bool startSave(const std::string& name);
	void deleteCfg(const std::string& file);
	HACK_INIT bool init();
	void reload();
	
	// file to load
	[[nodiscard]] std::string getCfgToLoad();
	// get all files
	[[nodiscard]] std::vector<std::string> getAllConfigFiles();
}