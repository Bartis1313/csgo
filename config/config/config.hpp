#pragma once

#include <filesystem>
#include <variant>
#include <vector>
#include <iostream>
#include <string>

#include "key.hpp"
#include "cfgcolor.hpp"
#include "cfgWeapon.hpp"

class ConfigType
{
public:
	// add possible types to accept
	using Types = std::variant<bool, int, float,
		std::string, CfgColor, std::vector<CfgWeapon>,
		std::vector<bool>, Key>;

	ConfigType() = default;
	/// <summary>
	/// Constructs the config type object, those objects should be init'ed temporiarly in some push
	/// </summary>
	/// <param name="var">variable to init field for type, the type will be checked if variant can accept it</param>
	/// <param name="name">name for save in JSON file</param>
	ConfigType(const Types var, const std::string& name);

	// get the value, template is needed to operate easier, by REFERENCE
	template<typename T>
	[[nodiscard]] T& getRef() const
	{
		return std::add_lvalue_reference_t<T>(std::get<T>(m_type));
	}
	// get the value, template is needed to operate easier, by VALUE
	template<typename T>
	[[nodiscard]] T get() const
	{
		return std::get<T>(m_type);
	}

	// setter for type
	template<typename T>
	void set(const T& var)
	{
		m_type = var;
	}
	// use only to return variant
	[[nodiscard]] Types getType() const { return m_type; }
	[[nodiscard]] std::string getName() const { return m_name; }
private:
	Types m_type;
	std::string m_name;
};

class Config
{
public:
	Config() = default;

	bool save(const std::string& file, const bool forceSave = false);
	bool saveCfgToLoad(const std::string& name);
	bool load(const std::string& file);
	void deleteCfg(const std::string& file);
	/// <summary>
	/// init the config
	/// </summary>
	/// <param name="defName">Default name of the config</param>
	/// <param name="defLoadFileName">Default name of the filename to gather load info</param>
	/// <param name="hackPath">Default path of where config is saved</param>
	/// <param name="loadPath">Default utility path for detection which cfg to load</param>
	/// <returns>true if success</returns>
	bool init(const std::string& defName, const std::string& defLoadFileName, const std::filesystem::path& hackPath, const std::filesystem::path& loadPath);
	void reload();
	// adds variable into the vector of variables, returns size due to getting "ID" from every varaible
	template <typename T>
	[[nodiscard]] size_t addVar(const T& var, const std::string& name)
	{
		m_allVars.emplace_back(ConfigType{ var, name });
		return m_allVars.size() - 1;
	}
	// get selected by REFERENCE variable
	template <typename T>
	[[nodiscard]] T& getRef(const size_t idx) const
	{
		return m_allVars.at(idx).getRef<T>();
	}
	// use to just check type value
	template <typename T>
	[[nodiscard]] T get(const size_t idx) const
	{
		return m_allVars.at(idx).get<T>();
	}
	// get load cfg name
	[[nodiscard]] std::string getCfgToLoad() const;
	// get main folder
	[[nodiscard]] std::filesystem::path getHackPath() const;
	// get main load folder
	[[nodiscard]] std::filesystem::path getLoadPath() const;
	// get DEFAULT name, returns the field m_defaultConfig which never change
	[[nodiscard]] std::string getDefaultConfigName() const { return m_defaultConfig; }
	// get correct path for savings
	[[nodiscard]] std::filesystem::path getPathForSave(const std::string& file) const;
	// get correct path for load names
	[[nodiscard]] std::filesystem::path getPathForLoad(const std::string& file) const;
	// get all files
	[[nodiscard]] std::vector<std::string> getAllConfigFiles() const { return m_allFilesInFolder; }
	// get documents path, static
	[[nodiscard]] static std::filesystem::path getDocumentsPath();
private:
	// don't duplicate names
	[[nodiscard]] size_t getIndexByName(const std::string& name);

	std::filesystem::path getPathForConfig(const std::string& file);

	std::vector<ConfigType> m_allVars;
	std::vector<std::string> m_allFilesInFolder;
	std::string m_defaultConfig;
	// filename where we read what to load
	std::string m_defaultFileNameLoad;
	
	std::filesystem::path m_path;
	std::filesystem::path m_loadPath;
};

inline Config config;