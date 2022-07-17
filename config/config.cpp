#include "config.hpp"

#include <shlobj.h>
#include <stdexcept>
#include <format>
#include <fstream>
#include <filesystem>

#include "../dependencies/json.hpp"
#include "../utilities/console/console.hpp"

using json = nlohmann::json;

ConfigType::ConfigType(const Types var, const std::string& name)
	: m_type{ var }, m_name{ name }
{}

bool Config::save(const std::string& file, const bool forceSave)
{
	if (file.empty())
	{
		console.log(TypeLogs::LOG_ERR, XOR("provided config name was empty"));
		return false;
	}

	if (utilities::toLowerCase(getPathForConfig(file).string()) == getDefaultConfigName() && !forceSave)
	{
		console.log(TypeLogs::LOG_ERR, XOR("provided config name was same as default"));
		return false;
	}

	const auto toWrite = utilities::toLowerCase(getPathForSave(getPathForConfig(file).string()).string());

	json config;

	for (const auto& var : m_allVars)
	{
		json entry;

		entry[XOR("name")] = var.getName();

		if (std::holds_alternative<bool>(var.getType()))
		{
			entry[XOR("value")] = var.getRef<bool>();
		}
		else if (std::holds_alternative<int>(var.getType()))
		{
			entry[XOR("value")] = var.getRef<int>();
		}
		else if (std::holds_alternative<float>(var.getType()))
		{
			entry[XOR("value")] = var.getRef<float>();
		}
		else if (std::holds_alternative<std::string>(var.getType()))
		{
			entry[XOR("value")] = var.getRef<std::string>();
		}
		else if (std::holds_alternative<CfgColor>(var.getType()))
		{
			auto col = var.get<CfgColor>();

			json arr =
			{
				col.getColor().r(),
				col.getColor().g(),
				col.getColor().b(),
				col.getColor().a(),
				col.getRainbow(),
				col.getSpeed()
			};

			entry[XOR("value")] = arr.dump();
		}
		else if (std::holds_alternative<std::vector<bool>>(var.getType()))
		{
			auto vec = var.get<std::vector<bool>>();

			json arr;

			for (bool el : vec)
				arr.push_back(el);

			entry[XOR("value")] = arr.dump();
		}
		else if (std::holds_alternative<Key>(var.getType()))
		{
			auto key = var.get<Key>();

			json arr =
			{
				key.getKeyMode(),
				key.getKeyCode()
			};

			entry[XOR("value")] = arr.dump();
		}

		config.push_back(entry);
	}

	std::ofstream stream{ toWrite };
	if (!stream)
		return false;

	try
	{
		stream << config.dump(4);
		stream.close();
	}
	catch (const std::ofstream::failure& err)
	{
		console.log(TypeLogs::LOG_ERR, FORMAT(XOR("Saving {} file has failed: {}"), file, err.what()));
	}

	console.log(TypeLogs::LOG_INFO, FORMAT(XOR("Saving {} file success"), file));
	return true;
}

bool Config::load(const std::string& file)
{
	const auto toWrite = getPathForSave(getPathForConfig(file).string());
	json config;

	std::ifstream input{ toWrite, std::ios::in };
	if (!input)
		return false;

	try
	{
		input >> config;
		input.close();
	}
	catch (const std::ifstream::failure& err)
	{
		console.log(TypeLogs::LOG_ERR, FORMAT(XOR("Loading {} file has failed: {}"), file, err.what()));
	}

	for (const auto& var : config)
	{
		size_t idx = getIndexByName(var[XOR("name")].get<std::string>());

		if (idx == -1)
			continue;

		auto& entry = m_allVars.at(idx);

		if (std::holds_alternative<bool>(entry.getType()))
		{
			entry.set(var[XOR("value")].get<bool>());
		}
		else if (std::holds_alternative<int>(entry.getType()))
		{
			entry.set(var[XOR("value")].get<int>());
		}
		else if (std::holds_alternative<float>(entry.getType()))
		{
			entry.set(var[XOR("value")].get<float>());
		}
		else if (std::holds_alternative<std::string>(entry.getType()))
		{
			entry.set(var[XOR("value")].get<std::string>());
		}
		else if (std::holds_alternative<CfgColor>(entry.getType()))
		{
			auto parsed = json::parse(var[XOR("value")].get<std::string>());

			entry.set(
				CfgColor(
					Color(
						parsed.at(0).get<float>(),
						parsed.at(1).get<float>(),
						parsed.at(2).get<float>(),
						parsed.at(3).get<float>()
					),
					parsed.at(4).get<bool>(),
					parsed.at(5).get<float>()
				)
			);
		}
		else if (std::holds_alternative<std::vector<bool>>(entry.getType()))
		{
			auto parsed = json::parse(var[XOR("value")].get<std::string>());
			auto& original = entry.getRef<std::vector<bool>>();

			for (size_t i = 0; i < original.size(); i++)
			{
				original.at(i) = parsed.at(i).get<bool>();
			}
		}
		else if (std::holds_alternative<Key>(entry.getType()))
		{
			auto parsed = json::parse(var[XOR("value")].get<std::string>());

			entry.set(
				Key(
					parsed.at(0).get<KeyMode>(),
					parsed.at(1).get<int>()
				)
			);
		}
	}

	console.log(TypeLogs::LOG_INFO, FORMAT(XOR("Loading {} file success"), file));
	return true;
}

std::filesystem::path Config::getPathForConfig(const std::string& file)
{
	std::filesystem::path path(file);
	if (path.extension() != XOR(".cfg"))
		path.replace_extension(XOR(".cfg"));

	return path;
}

size_t Config::getIndexByName(const std::string& name)
{
	for (size_t i = 0; i < m_allVars.size(); i++)
	{
		if (m_allVars.at(i).getName() == name)
			return i;
	}

	return -1;
}

bool Config::init(const std::string& defName, const std::string& defLoadFileName,
	const std::filesystem::path& hackPath, const std::filesystem::path& loadPath)
{
	m_defaultConfig = defName;
	m_defaultFileNameLoad = defLoadFileName;
	m_path = hackPath;
	m_loadPath = loadPath;

	// check if the path to where save files is even a directory
	if (auto path = getHackPath(); !std::filesystem::is_directory(path))
	{
		// if no, remove it, in
		std::filesystem::remove(path);

		// after removal, create the folder, from there the path is possible to reach
		if (!std::filesystem::create_directories(path))
			return false;
	}

	// same thing in load
	if (auto path = getLoadPath(); !std::filesystem::is_directory(path))
	{
		// if no, remove it, in
		std::filesystem::remove(path);

		// after removal, create the folder, from there the path is possible to reach
		if (!std::filesystem::create_directories(path))
			return false;
	}

	// check if the default file already exists, if yes, don't save
	// TODO: detect any changes, replace them with new file. Only idea for now is to compare file size (kb)
	if (auto path = getPathForSave(getCfgToLoad()); !std::filesystem::exists(path))
	{
		console.log(TypeLogs::LOG_INFO, FORMAT(XOR("Creating new file, because it doesn't exist: {}"), path.string()));

		if (!save(getCfgToLoad(), true))
			return false;
	}

	if (!load(getCfgToLoad()))
	{
		return false;
	}

	// push file names with .cfg into vector
	reload();

	return true;
}

std::string Config::getCfgToLoad() const
{
	// unsafe operation, nobody edits it manually anyway
	std::ifstream read{ getPathForLoad(m_defaultFileNameLoad) };
	if (!read) // if not created
	{
		std::ofstream write{ getPathForLoad(m_defaultFileNameLoad) };
		write << getDefaultConfigName();
		write.close();
	}

	std::string str{ std::istreambuf_iterator<char>{ read }, {} };
	read.close();
	return str;
}

bool Config::saveCfgToLoad(const std::string& name)
{
	std::ofstream write{ getPathForLoad(m_defaultFileNameLoad) };
	if (!write)
		return false;

	write << name;
	write.close();

	return true;
}

void Config::reload()
{
	m_allFilesInFolder.clear();
	auto iterator = std::filesystem::directory_iterator(getHackPath());
	for (const auto& entry : iterator)
	{
		if (std::string name = entry.path().filename().string();
			entry.path().extension() == XOR(".cfg") && !name.empty())
		{
			m_allFilesInFolder.push_back(name);
		}
	}
}

std::filesystem::path Config::getDocumentsPath()
{
#ifdef _DEBUG
	// if possible to get the path, if so, return it
	if (static CHAR documents[MAX_PATH]; SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents)))
		return std::filesystem::path{ documents };
#else
	if (static CHAR documents[MAX_PATH]; SUCCEEDED(LF(SHGetFolderPathA).cached()(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents)))
		return std::filesystem::path{ documents };
#endif
	return {};
}

std::filesystem::path Config::getHackPath() const
{
	assert(getDocumentsPath().empty() == false && "hacks path was empty");

	std::filesystem::path toReturn;
	toReturn.assign(getDocumentsPath() / m_path);
	return toReturn;
}

std::filesystem::path Config::getLoadPath() const
{
	assert(getDocumentsPath().empty() == false && "load cfg path was empty");

	std::filesystem::path toReturn;
	toReturn.assign(getDocumentsPath() / m_loadPath);
	return toReturn;
}

std::filesystem::path Config::getPathForSave(const std::string& file) const
{
	return std::filesystem::path{ getHackPath() / file };
}

std::filesystem::path Config::getPathForLoad(const std::string& file) const
{
	return std::filesystem::path{ getLoadPath() / file };
}

void Config::deleteCfg(const std::string& file)
{
	auto path = getPathForConfig(file);

	if (path.string() == m_defaultConfig)
	{
		console.log(TypeLogs::LOG_ERR, XOR("Can't delete default config"));
		return;
	}

	if (auto toDel = getPathForSave(path.string()); std::filesystem::remove(toDel))
		console.log(TypeLogs::LOG_INFO, FORMAT(XOR("Removed config {}"), toDel.filename().string()));
}