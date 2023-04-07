#include "config.hpp"

#include "vars.hpp"
#include "jsonExtended.hpp"

#include <shlobj.h>
#include <stdexcept>
#include <format>
#include <fstream>
#include <filesystem>

#include <utilities/utilities.hpp>
#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <render/Color.hpp>

bool Config::save(const std::string& file)
{
	if (file.empty())
	{
		console::error("provided config name was empty");
		return false;
	}

	json j;

	to_json(j["Aim"], *vars::aim);
	to_json(j["AimPaint"], *vars::aimPaint);
	to_json(j["Visuals"], *vars::visuals);
	to_json(j["Keys"], *vars::keys);
	to_json(j["Backtrack"], *vars::backtrack);
	to_json(j["Misc"], *vars::misc);
	to_json(j["Styling"], *vars::styling);

	try
	{
		std::ofstream out{ utilities::toLowerCase((getHackPath() / getPathForConfig(file)).string()) };
		if (!out)
			return false;

		out << std::setw(4) << j;
	}
	catch (const std::ofstream::failure& err)
	{
		console::error("Saving {} file has failed: {}", file, err.what());
	}

	console::debug("Saving file {}", file);

	return true;
}

bool Config::load(const std::string& file)
{
	std::ifstream input{ getHackPath() / getPathForConfig(file) };
	if (!input)
		return false;

	json j = json::parse(input);

	from_json(j["Aim"], *vars::aim);
	from_json(j["AimPaint"], *vars::aimPaint);
	from_json(j["Visuals"], *vars::visuals);
	from_json(j["Keys"], *vars::keys);
	from_json(j["Backtrack"], *vars::backtrack);
	from_json(j["Misc"], *vars::misc);
	from_json(j["Styling"], *vars::styling);

	console::debug("Loading file {}", file);

	return true;
}

std::filesystem::path Config::getPathForConfig(const std::string& file)
{
	std::filesystem::path path(file);
	if (path.extension() != ".cfg")
		path.replace_extension(".cfg");

	return path;
}

bool Config::init(const std::string& defName, const std::string& defLoadFileName, const std::filesystem::path& loadPath)
{
	m_defaultConfig = defName;
	m_defaultFileNameLoad = defLoadFileName;
	m_loadExtraPath = loadPath;

	// check if the path to where save files is even a directory
	if (auto path = getHackPath(); !std::filesystem::exists(path))
	{
		// after removal, create the folder, from there the path is possible to reach
		if (!std::filesystem::create_directories(path))
			return false;
	}

	// same thing in load
	if (auto path = getHackPath() / m_loadExtraPath; !std::filesystem::exists(path))
	{
		// after removal, create the folder, from there the path is possible to reach
		if (!std::filesystem::create_directories(path))
			return false;
	}

	// default file doesn't exist
	if (auto path = getHackPath() / m_defaultConfig; !std::filesystem::exists(path))
	{
		console::warn("Creating default file, because it doesn't exist: {}", path.string());

		if (!save(m_defaultConfig))
			return false;
	}

	// loading file doesnt exists
	if (auto path = getHackPath() / m_loadExtraPath / m_defaultFileNameLoad ; !std::filesystem::exists(path))
	{
		console::warn("Creating loading file, because it doesn't exist: {}", path.string());

		if (!startSave(m_defaultConfig))
			return false;
	}

	if (!startLoad())
		return false;

	// loaded file exists but config file is gone, then cleanup
	if (auto path = getHackPath() / loadedCfgName; !std::filesystem::exists(path))
	{
		console::warn("Creating loaded file, because it doesn't exist: {}", path.string());

		if (!save(loadedCfgName))
			return false;
	}

	if (!load(loadedCfgName))
		return false;

	// push file names with .cfg into vector
	reload();

	return true;
}

bool Config::startLoad()
{
	std::ifstream input{ getHackPath() / m_loadExtraPath / m_defaultFileNameLoad };
	if (!input)
		return false;

	json j = json::parse(input);

	from_json(j, "Name", loadedCfgName);

	return true;
}

bool Config::startSave(const std::string& name)
{
	json j;
	j["Name"] = name;

	std::ofstream out{ getHackPath() / m_loadExtraPath / m_defaultFileNameLoad };
	if (!out)
		return false;

	out << j;

	return true;
}

void Config::reload()
{
	m_allFilesInFolder.clear();
	auto iterator = std::filesystem::directory_iterator(getHackPath());
	for (const auto& entry : iterator)
	{
		if (std::string name = entry.path().filename().string();
			entry.path().extension() == ".cfg" && !name.empty())
		{
			m_allFilesInFolder.push_back(name);
		}
	}
}

std::filesystem::path Config::getDocumentsPath()
{
	if (static CHAR documents[MAX_PATH]; SUCCEEDED(LI_FN_CACHED(SHGetFolderPathA)(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents)))
		return std::filesystem::path{ documents };

	return {};
}

std::filesystem::path Config::getHackPath() const
{
	assert(getDocumentsPath().empty() == false && "hacks path was empty");

	std::filesystem::path toReturn;
	toReturn.assign(getDocumentsPath() / m_path);
	return toReturn;
}

void Config::deleteCfg(const std::string& file)
{
	auto path = getPathForConfig(file);

	if (path.string() == m_defaultConfig)
	{
		console::error("Can't delete default config");
		return;
	}

	if (auto toDel = getHackPath() / path; std::filesystem::remove(toDel))
		console::debug("Removed config {}", toDel.filename().string());
}