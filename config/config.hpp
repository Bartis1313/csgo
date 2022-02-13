#pragma once
#include "../dependencies/json.hpp"
#include "../utilities/utilities.hpp"
#include "../SDK/Color.hpp"
#include <variant>
#include <vector>
#include <iostream>

class ConfigType
{
public:
	// add possible types to accept
	using Types = std::variant<bool, int, float,
		std::string, Color,
		std::vector<bool>>;

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
	/// <summary>
	/// Constructs default config
	/// </summary>
	/// <param name="file">default file name to get config from</param>
	/// <param name="folder"></param>
	Config(const std::string& file, const std::string& folder);

	bool save(const std::string& file);
	bool load(const std::string& file);
	// pass default file here
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
	// getting this field will be needed to list them or count
	_NODISCARD std::vector<std::string>& getAllConfigFiles() const;
	// get main folder
	_NODISCARD std::filesystem::path getHacksPath() const;
	// usefull in case of x88 menu
	_NODISCARD std::string getDefaultConfigName() const { return m_defaultName; }
	// get correct path for savings
	_NODISCARD std::filesystem::path getPathForSave(const std::string& file) const;
private:
	// don't duplicate names
	_NODISCARD size_t getIndexByName(const std::string& name);

	std::filesystem::path getPathForConfig(const std::string& file);
	std::filesystem::path m_documentsPath;

	std::vector<ConfigType> m_allVars;
	std::vector<std::string> m_allFilesInFolder;
	std::string m_folder;
	std::string m_defaultName;
	// DO NOT CHANGE
	const std::string m_defaultConfig = XOR("default.cfg");
	// your desire to name it
	static std::string m_mainEntryFolder;
} inline config{ XOR("default.cfg"), XOR("csgo") }; /*should be stored somewhere else for many places and so instances
													 also iif you want "load this config on start" thing, just pass the name to constructor*/