#include "config.hpp"
#include <shlobj.h>
#include <stdexcept>
#include <format>
#include <fstream>
#include <filesystem>

using namespace nlohmann;

std::string Config::m_mainEntryFolder = XOR("Bartis_internal");

ConfigType::ConfigType(const Types var, const std::string& name)
	: m_type{ var }, m_name{ name }
{
	// now check if the type is correct
	if (std::holds_alternative<bool>(m_type))
		m_isGoodType = true;
	else if (std::holds_alternative<int>(m_type))
		m_isGoodType = true;
	else if (std::holds_alternative<float>(m_type))
		m_isGoodType = true;
	else if (std::holds_alternative<std::string>(m_type))
		m_isGoodType = true;
	else if (std::holds_alternative<Color>(m_type))
	{
		m_isGoodType = true;
		m_isColor = true;
	}
	else if (std::holds_alternative<std::vector<bool>>(m_type))
	{
		m_isGoodType = true;
		m_isVec = true;
	}
	else
		m_isGoodType = false;

	if (!m_isGoodType)
		throw std::runtime_error(XOR("unknown type, check the variant"));
}

Config::Config(const std::string& file, const std::string& folder)
	: m_defaultName{ file }, m_folder{ folder }
{}

bool Config::save(const std::string& file)
{
	const auto toWrite = getPathForSave(getPathForConfig(file).string());
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
		else if (std::holds_alternative<Color>(var.getType()))
		{
			auto col = var.getRef<Color>();

			json arr =
			{
				col.r(),
				col.g(),
				col.b(),
				col.a()
			};

			entry[XOR("value")] = arr.dump();
		}
		else if (std::holds_alternative<std::vector<bool>>(var.getType()))
		{
			auto vec = var.getRef<std::vector<bool>>();

			json arr;

			for (bool el : vec)
				arr.emplace_back(std::move(el));

			entry[XOR("value")] = arr.dump();
		}

		config.emplace_back(std::move(entry));
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
		LOG(LOG_ERR, std::format(XOR("Saving {} file has failed: {}"), file, err.what()));
	}

	LOG(LOG_INFO, std::format(XOR("Saving {} file success"), file));
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
		LOG(LOG_ERR, std::format(XOR("Loading {} file has failed: {}"), file, err.what()));
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
		else if (std::holds_alternative<Color>(entry.getType()))
		{
			auto parsed = json::parse(var[XOR("value")].get<std::string>());

			entry.set(
				Color(
					parsed.at(0).get<uint8_t>(),
					parsed.at(1).get<uint8_t>(),
					parsed.at(2).get<uint8_t>(),
					parsed.at(3).get<uint8_t>()
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
	}

	LOG(LOG_INFO, std::format(XOR("Loading {} file success"), file));
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

bool Config::init()
{
	if (auto path = getHacksPath(); !path.empty())
		m_documentsPath = path;
	else
	{
		// throw err, it's something not OK to happen
		throw std::runtime_error(XOR("documents path could not be reached"));
	}

	// check if the path to where save files is even a directory
	if (auto path = m_documentsPath / m_folder; !std::filesystem::is_directory(path))
	{
		// if no, remove it, in
		std::filesystem::remove(path);

		// after removal, create the folder, from there the path is possible to reach
		if (!std::filesystem::create_directories(path))
			return false;
	}

	// check if the default file already exists, if yes, don't save
	// TODO: detect any changes, replace them with new file. Only idea for now is to compare file size (kb)
	if (auto path = m_documentsPath / m_folder / getDefaultConfigName(); !std::filesystem::exists(path))
	{
		LOG(LOG_INFO, std::format(XOR("Creating new file, because it doesn't exist: {}"), path.string()));

		if (!save(getDefaultConfigName()))
			return false;
	}

	if (!load(getDefaultConfigName()))
	{
		return false;
	}

	// push file names with .cfg into vector
	reload();

	return true;
}


void Config::reload()
{
	m_allFilesInFolder.clear();
	auto iterator = std::filesystem::directory_iterator(m_documentsPath / m_folder);
	for (const auto& entry : iterator)
	{
		if (std::string name = entry.path().filename().string();
			entry.path().extension() == XOR(".cfg") && !name.empty())
		{
			m_allFilesInFolder.emplace_back(std::move(name));
		}
	}
}

std::filesystem::path Config::getHacksPath() const
{
	// if possible to get the path, if so, return it
	if (CHAR documents[MAX_PATH]; SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents)))
	{
		std::filesystem::path toReturn;
		toReturn.assign(documents);
		toReturn.append(m_mainEntryFolder);
		return toReturn;
	}
	// if fail, return empty
	return {};
}

std::filesystem::path Config::getPathForSave(const std::string& file) const
{
	return std::filesystem::path{ m_documentsPath / m_folder / file };
}

void Config::deleteCfg(const std::string& file)
{
	auto path = getPathForConfig(file);

	if (path.string() == m_defaultConfig)
		LOG(LOG_ERR, XOR("Can't delete default config"));

	if (auto toDel = getPathForSave(path.string()); std::filesystem::remove(toDel))
		LOG(LOG_INFO, std::format(XOR("Removed config {}"), toDel.filename().string()));
}