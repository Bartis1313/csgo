#include "editor.hpp"

#include <classes/renderableToPresent.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/tools/tools.hpp>
#include <menu/GUI-ImGui/animations.hpp>
#include <dependencies/ImGui/imgui_stdlib.h>
#include <dependencies/magic_enum.hpp>
#include <config/jsonExtended.hpp>
#include <config/config.hpp>

#include <ranges>
#include <fstream>

using json = nlohmann::json;

void from_json(const json& j, Chams::Mat_t::Data& val)
{
	from_json(j, "Name", val.name);
	from_json(j, "Key", val.key);
	from_json(j, "Buf", val.buf);
}

void to_json(json& j, const Chams::Mat_t::Data& val)
{
	j["Name"] = val.name;
	j["Key"] = val.key;
	j["Buf"] = val.buf;
}

void from_json(const json& j, Chams::Mat_t& val)
{
	from_json(j, "Data", val.data);
	from_json(j, "Type", val.type);
}

void to_json(json& j, const Chams::Mat_t& val)
{
	j["Data"] = val.data;
	j["Type"] = val.type;
}

// use this correctly
// read: https://developer.valvesoftware.com/wiki/Category:Shaders
// and lists, in this case you unortunately have to copy key 
// WIPPPPPP, ugly code
void MaterialEditor::draw()
{
	if (!m_open)
		return;
	// [SECTION] Example App: Simple Layout / ShowExampleAppLayout() demo ref
	ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Material editor", &m_open, ImGuiWindowFlags_NoCollapse))
	{
		static size_t index = m_oldIndex;
		static Chams::Mat_t material = getMaterialIndexed(index).value_or(Chams::Mat_t{});
		{
			ImGui::BeginChild(XOR("left pane"), ImVec2{ 150.0f, 0.0f }, true);

			for (auto i : std::views::iota(m_oldIndex, g_Chams->m_materials.size()))
			{
				if (ImGui::Animations::Selectable(g_Chams->m_materials.at(i).data.name.data(), index == i))
				{
					index = i;
					material = getMaterialIndexed(index).value();
					m_ImEditor.SetText(material.data.buf);
				}
			}


			ImGui::EndChild();
		}
		ImGui::SameLine();


		ImGui::BeginChild(XOR("Materials view"));

		if (g_Chams->m_materials.size() > m_oldIndex)
			ImGui::TextUnformatted(FORMAT(XOR("{}"), g_Chams->m_materials.at(index).data.name).c_str());
		else
			ImGui::TextUnformatted(XOR("No materials yet, add one!"));
		ImGui::Separator();

		ImGui::InputText(XOR("Name"), &material.data.name);
		ImGui::InputText(XOR("Key"), &material.data.key);
		ImGui::Animations::Combo<const std::string>(XOR("Style"), reinterpret_cast<int*>(&material.type), magic_enum::enum_names_pretty<Chams::Mat_t::ExtraType>());

		if (ImGui::Animations::Button(XOR("Delete")))
		{
			size_t elementsRemoved = std::erase_if(g_Chams->m_materials,
				[this](const auto& m)
				{
					if (m.isFromEditor && m.data.name == material.data.name)
					{
						return true;
					}
					else
					{
						return false;
					}
				});
			if (elementsRemoved > 0)
			{
				index--;
				size_t correctIndex = g_Chams->m_materials.size() - elementsRemoved;
				material = getMaterialIndexed(index).value_or(Chams::Mat_t{});
				m_json.erase(m_json.find(material.data.name));
				saveCfg();
				m_ImEditor.SetText(material.data.name);
			}
		}
		ImGui::SameLine();
		if (ImGui::Animations::Button(XOR("Add")))
		{
			const auto mat = g_Chams->addMaterialByBuffer(Chams::Mat_t{ .isFromEditor = true, .data = Chams::Mat_t::Data{.name = material.data.name, .key = material.data.key,
				.buf = m_ImEditor.GetText() } });
			if (mat.has_value())
			{
				g_Chams->m_materials.emplace_back(mat.value());
				saveCfg();
			}
		}
		ImGui::SameLine();
		if (ImGui::Animations::Button(XOR("Update")))
		{
			if (const auto itr = std::ranges::find_if(g_Chams->m_materials,
				[=](const auto& m)
				{
					if (m.data.name == material.data.name)
					{
						return true;
					}
					else
					{
						return false;
					}
				}); itr != g_Chams->m_materials.end())
			{
				*itr = Chams::Mat_t{ .data = Chams::Mat_t::Data{.name = material.data.name, .key = material.data.key, .buf = m_ImEditor.GetText() } };
			}
		}

		m_ImEditor.Render(XOR("Editor##mat"));

		ImGui::EndChild();

		ImGui::End();
	}
}

void MaterialEditor::initEditor()
{
	m_folderName = XOR("materials");
	m_saveDir = config.getHackPath() / config.getExtraLoadPath() / m_folderName / getPathForConfig();

	m_oldIndex = g_Chams->m_materials.size();
	loadCfg();

	for (auto i : std::views::iota(m_oldIndex, g_Chams->m_materials.size()))
		g_Chams->m_materials.at(i) = g_Chams->addMaterialByBuffer(g_Chams->m_materials.at(i), false).value();
}

bool MaterialEditor::loadCfg()
{
	if (auto path = config.getHackPath() / config.getExtraLoadPath() / m_folderName; !std::filesystem::exists(path))
		std::filesystem::create_directories(path);

	std::ifstream input{ m_saveDir };
	if (!input)
		return false;

	if (!std::filesystem::is_empty(m_saveDir))
	{
		m_json = json::parse(input);

		for (const auto& [key, value] : m_json.items())
		{
			Chams::Mat_t material;
			from_json(value, material);
			material.isFromEditor = true;

			g_Chams->m_materials.push_back(material);
		}
	}

	return true;
}

bool MaterialEditor::saveCfg()
{
	std::ofstream out{ m_saveDir };
	if (!out)
		return false;

	json j;
	for (const auto& el : g_Chams->m_materials)
	{
		if(el.isFromEditor)
			to_json(j[el.data.name], el);
	}

	if (!j.empty())
		m_json.update(j);

	out << std::setw(4) << m_json;

	return true;
}

std::filesystem::path MaterialEditor::getPathForConfig() const
{
	std::filesystem::path path(m_folderName);
	if (path.extension() != XOR(".json"))
		path.replace_extension(XOR(".json"));

	return path;
}

std::optional<Chams::Mat_t> MaterialEditor::getMaterialIndexed(size_t index) const
{
	if (index < m_oldIndex)
		return std::nullopt;

	if (index >= g_Chams->m_materials.size())
		return std::nullopt;

	const auto material = g_Chams->m_materials.at(index);
	return Chams::Mat_t{ .data = Chams::Mat_t::Data{.name = material.data.name, .key = material.data.key, .buf = material.data.buf }, .type = material.type };
}