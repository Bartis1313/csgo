#pragma once

#include <filesystem>
#include <variant>
#include <vector>
#include <iostream>

#include "key.hpp"
#include "../utilities/utilities.hpp"
#include "../SDK/Color.hpp"

class ConfigType
{
public:
	// add possible types to accept
	using Types = std::variant<bool, int, float,
		std::string, Color,
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
	_NODISCARD T& getRef() const
	{
		// only check on debug, who would fail for this?
#ifdef _DEBUG
		if (!std::holds_alternative<T>(m_type))
			throw std::runtime_error(XOR("unknown type, check the variant and given template type"));
#endif
		return std::add_lvalue_reference_t<T>(std::get<T>(m_type));
	}
	// get the value, template is needed to operate easier, by VALUE
	template<typename T>
	_NODISCARD T get() const
	{
#ifdef _DEBUG
		if (!std::holds_alternative<T>(m_type))
			throw std::runtime_error(XOR("unknown type, check the variant and given template type"));
#endif
		return std::get<T>(m_type);
	}

	// setter for type
	template<typename T>
	void set(const T& var)
	{
		m_type = var;
	}
	// use only to return variant
	_NODISCARD Types getType() const { return m_type; }
	_NODISCARD std::string getName() const { return m_name; }
private:
	Types m_type;
	std::string m_name;
	bool m_isGoodType = false;
	bool m_isColor = false;
	bool m_isVec = false;
};

class Config
{
public:
	Config() = default;

	bool save(const std::string& file, const bool forceSave = false);
	bool load(const std::string& file);
	void deleteCfg(const std::string& file);
	// will init the config for default cfg
	bool init();
	void reload();
	// adds variable into the vector of variables, returns size due to getting "ID" from every varaible
	template<typename T>
	_NODISCARD size_t addVar(const T& var, const std::string& name)
	{
		m_allVars.emplace_back(std::move(ConfigType{ var, name }));
		return m_allVars.size() - 1;
	}
	// get selected by REFERENCE variable
	template <typename T>
	_NODISCARD T& getRef(const size_t idx) const
	{
		return m_allVars.at(idx).getRef<T>();
	}
	// use to just check type value
	template <typename T>
	_NODISCARD T get(const size_t idx) const
	{
		return m_allVars.at(idx).get<T>();
	}
	// get main folder
	_NODISCARD std::filesystem::path getHacksPath() const;
	// get DEFAULT name, returns the field m_defaultConfig which never change
	_NODISCARD std::string getDefaultConfigName() const { return m_defaultConfig; }
	// get correct path for savings
	_NODISCARD std::filesystem::path getPathForSave(const std::string& file) const;
	// get all files
	_NODISCARD std::vector<std::string> getAllConfigFiles() const { return m_allFilesInFolder; }
private:
	// don't duplicate names
	_NODISCARD size_t getIndexByName(const std::string& name);

	std::filesystem::path getPathForConfig(const std::string& file);
	std::filesystem::path m_documentsPath;

	std::vector<ConfigType> m_allVars;
	std::vector<std::string> m_allFilesInFolder;
	// your desire to name it
	const std::string m_defaultConfig = XOR("default.cfg");
	const std::string m_folder = XOR("csgo");
	const std::string m_mainEntryFolder = XOR("Bartis_internal");
};

inline Config config;