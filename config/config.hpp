#pragma once

#include <filesystem>
#include <vector>
#include <iostream>
#include <string>

#include "key.hpp"
#include "cfgcolor.hpp"
#include "cfgWeapon.hpp"

class Config
{
public:
	Config() = default;
	bool startLoad();
	bool startSave(const std::string& name);

	void setFolder(const std::filesystem::path& hackPath) { m_path = hackPath; }
	bool save(const std::string& file);
	bool load(const std::string& file);
	void deleteCfg(const std::string& file);
	/// <summary>
	/// init the config
	/// </summary>
	/// <param name="defName">Default name of the config</param>
	/// <param name="defLoadFileName">Default name of the filename to gather load info</param>
	/// <param name="hackPath">Default path of where config is saved</param>
	/// <param name="loadPath">Default utility path for detection which cfg to load
	/// Full path is hackPath / hackPatch, meaning you are fine with passing folder name only</param>
	/// <returns>true if success</returns>
	bool init(const std::string& defName, const std::string& defLoadFileName, const std::filesystem::path& loadPath);
	void reload();
	
	// get main folder
	[[nodiscard]] std::filesystem::path getHackPath() const;
	// get all files
	[[nodiscard]] std::vector<std::string> getAllConfigFiles() const { return m_allFilesInFolder; }
	// get documents path, static
	[[nodiscard]] static std::filesystem::path getDocumentsPath();
	// file to load
	[[nodiscard]] std::string getCfgToLoad() const { return loadedCfgName; }
	// get DEFAULT name, returns the field m_defaultConfig which never change
	[[nodiscard]] std::string getDefaultConfigName() const { return m_defaultConfig; }
	// extra folder for utilities
	[[nodiscard]] std::filesystem::path getExtraLoadPath() const { return m_loadExtraPath; }
private:
	[[nodiscard]] std::filesystem::path getPathForConfig(const std::string& file);

	std::vector<std::string> m_allFilesInFolder;
	std::string m_defaultConfig;
	std::string m_defaultFileNameLoad;
	std::string loadedCfgName;	
	std::filesystem::path m_path;
	std::filesystem::path m_loadExtraPath;
};

inline Config config;