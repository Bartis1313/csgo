#include "editor.hpp"

#include <cheats/classes/renderableToPresent.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/tools/tools.hpp>
#include <menu/GUI-ImGui/animations.hpp>
#include <menu/GUI-ImGui/imguiaddons.hpp>
#include <imgui_stdlib.h>
#include <deps/ImGui/imgui_markdown.h>
#include <deps/magic_enum/prettyNames.hpp>
#include <config/jsonExtended.hpp>
#include <config/config.hpp>
#include <SDK/material.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/KeyValues.hpp>
#include <SDK/KeyValuesSys.hpp>
#include <gamememory/memory.hpp>

#include <ranges>
#include <fstream>
#include <Shellapi.h>

using json = nlohmann::json;

void from_json(const json& j, Mat_t::Data& val)
{
	from_json(j, "Name", val.name);
	from_json(j, "Key", val.key);
	from_json(j, "Buf", val.buf);
}

void to_json(json& j, const Mat_t::Data& val)
{
	j["Name"] = val.name;
	j["Key"] = val.key;
	j["Buf"] = val.buf;
}

void from_json(const json& j, Mat_t& val)
{
	from_json(j, "Data", val.data);
	from_json(j, "Type", val.type);
	from_json(j, "Strategy", val.strategy);
}

void to_json(json& j, const Mat_t& val)
{
	j["Data"] = val.data;
	j["Type"] = val.type;
	j["Strategy"] = val.strategy;
}

void callbackForMarkdown(ImGui::MarkdownLinkCallbackData data_)
{
	std::string url{ data_.link, static_cast<size_t>(data_.linkLength) };
	if (!data_.isImage)
	{
		ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}

static ImGui::MarkdownConfig mdConfig;
void markdownEditor(const std::string& markdown_)
{
	mdConfig.linkCallback = callbackForMarkdown;
	ImGui::Markdown(markdown_.c_str(), markdown_.length(), mdConfig);
}

void MaterialEditor::draw()
{
	if (!m_open)
		return;

	constexpr auto popupTitle = "Material error";

	// [SECTION] Example App: Simple Layout / ShowExampleAppLayout() demo ref
	ImGui::SetNextWindowSize(ImVec2{ 500.0f, 440.0f }, ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Material editor", &m_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar))
	{
		renderHelpBar();
		renderAddMaterial();
		renderMaterialsList();
		renderMaterialsView();

		ImGui::End();
	}
}

void MaterialEditor::renderHelpBar()
{
	if (ImGui::BeginMenuBar())
	{
		ImGui::SetNextWindowSize(ImVec2{ 250.0f, 0.0f });
		if (ImGui::BeginMenu("Help"))
		{
			const std::string markdown_text = R"(First, read basics written [here](https://developer.valvesoftware.com/wiki/KeyValues)
Second, read about [materials](https://developer.valvesoftware.com/wiki/Material)
**CTRL-S as a shortcut adds / updates current material**)";
			markdownEditor(markdown_text);

			ImGui::EndMenu();
		}

		if (ImGui::Button("Add##material"))
		{
			m_subAddOpen = true;
		}

		ImGui::EndMenuBar();
	}
}

void MaterialEditor::renderAddMaterial()
{
	if (m_subAddOpen)
	{
		if (ImGui::Begin("New material", &m_subAddOpen, ImGuiWindowFlags_NoCollapse))
		{
			renderMaterialInfo(m_materialToAdd);

			if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
			{
				m_materialToAdd.data.buf = m_ImEdtorSubAdd.GetText();
				if (isMaterialValid(m_materialToAdd, false))
				{

					std::optional<Mat_t> mat;

					if (m_materialToAdd.strategy == Mat_t::StrategyType::FROM_STRING)
					{
						// prob good to split the \n 's here ?
						mat = g_Chams->addMaterialByString(Mat_t
							{
								.isFromEditor = true, .type = m_materialToAdd.type, .strategy = m_materialToAdd.strategy,
								.data = Mat_t::Data{.name = m_materialToAdd.data.name, .key = m_materialToAdd.data.key, .buf = m_ImEdtorSubAdd.GetText() }
							});
					}
					else
					{
						mat = g_Chams->addMaterialByBuffer(Mat_t
							{
								.isFromEditor = true, .type = m_materialToAdd.type, .strategy = m_materialToAdd.strategy,
								.data = Mat_t::Data{.name = m_materialToAdd.data.name, .key = m_materialToAdd.data.key, .buf = m_ImEdtorSubAdd.GetText() }
							});
					}

					if (mat.has_value())
					{
						g_Chams->m_materials.emplace_back(mat.value());

						m_currectIndex = g_Chams->m_materials.size() - 1;
						m_material = getMaterialIndexed(m_currectIndex).value_or(Mat_t{});
						m_ImEditor.SetText(m_material.data.buf);

						saveCfgIndex();

						ImNotify::add(ImNotify{ 5.0f, "Added material", std::format("Name {} Key {}\nBuf length {}", m_material.data.name, m_material.data.key, m_material.data.buf.length()) });

						m_subAddOpen = false;
					}
				}
				else
					ImNotify::add(ImNotify{ 5.0f, "Error adding material", "Please check duplication or you left inputs empty" });
			}

			m_ImEdtorSubAdd.Render("Material to add");

			ImGui::End();
		}
	}
}

void MaterialEditor::renderMaterialInfo(Mat_t& mat)
{
	ImGui::TextUnformatted("Strategy");
	ImGui::RadioButton("Buffer", reinterpret_cast<int*>(&mat.strategy), 0);
	ImGui::SameLine();
	ImGui::RadioButton("From String", reinterpret_cast<int*>(& mat.strategy), 1);
	ImGui::InputText("Name", &mat.data.name);
	ImGui::InputText("Key", &mat.data.key);
	ImGui::Animations::Combo("Style", reinterpret_cast<int*>(&mat.type), magic_enum::enum_names_pretty<Mat_t::ExtraType>());
}

void MaterialEditor::renderMaterialsList()
{
	ImGui::BeginChild("left pane", ImVec2{ 150.0f, 0.0f }, true);
	{
		for (auto i : std::views::iota(m_oldIndex, g_Chams->m_materials.size()))
		{
			if (ImGui::Animations::Selectable(g_Chams->m_materials.at(i).data.name.data(), m_currectIndex == i))
			{
				// cache text
				g_Chams->m_materials.at(m_currectIndex).data.buf = m_ImEditor.GetText();
				m_currectIndex = i;
				m_ImEditor.SetText(g_Chams->m_materials.at(m_currectIndex).data.buf);
				m_material = g_Chams->m_materials.at(i);
				m_material.data.buf = m_ImEditor.GetText();
			}
		}

		ImGui::EndChild();
	}
	ImGui::SameLine();
}

void MaterialEditor::renderMaterialsView()
{
	ImGui::BeginChild("Materials view");
	{
		if (g_Chams->m_materials.size() > m_oldIndex)
			ImGui::TextUnformatted(std::format("{}", g_Chams->m_materials.at(m_currectIndex).data.name).c_str());
		else
			ImGui::TextUnformatted("No materials yet, add one!");


		ImGui::SameLine();
		if (ImGui::Animations::Button("Delete"))
		{
			const size_t elementsRemoved = std::erase_if(g_Chams->m_materials,
				[this](const auto& m)
				{
					if (m.isFromEditor && m.data.name == m_material.data.name)
					{
						m_json.erase(m_json.find(m_material.data.name));
						return true;
					}
					else
					{
						return false;
					}
				});
			if (elementsRemoved > 0)
			{
				saveCfg();
				m_currectIndex -= elementsRemoved;
				m_material = getMaterialIndexed(m_currectIndex).value_or(Mat_t{});
				m_ImEditor.SetText(m_material.data.buf);

				ImNotify::add(ImNotify{ 5.0f, "Deleted material", std::format("Name {} Key {}\nBuf length {}", m_material.data.name, m_material.data.key, m_material.data.buf.length())});
			}
			else
			{
				ImNotify::add(ImNotify{ 5.0f, "Attemting to delete nothing", "Please add any material" });
			}
		}

		ImGui::Separator();

		renderMaterialInfo(m_material);

		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
		{
			const bool validmaterial = isMaterialValid(m_material, true);

			if (validmaterial)
			{

				std::optional<Mat_t> mat;

				if (m_material.strategy == Mat_t::StrategyType::FROM_STRING)
				{
					// prob good to split the \n 's here ?
					mat = g_Chams->addMaterialByString(Mat_t
						{
							.isFromEditor = true, .type = m_material.type, .strategy = m_material.strategy,
							.data = Mat_t::Data{.name = m_material.data.name, .key = m_material.data.key, .buf = m_ImEditor.GetText() }
						});
				}
				else
				{
					mat = g_Chams->addMaterialByBuffer(Mat_t
						{
							.isFromEditor = true, .type = m_material.type, .strategy = m_material.strategy,
							.data = Mat_t::Data{.name = m_material.data.name, .key = m_material.data.key, .buf = m_ImEditor.GetText() }
						});
				}


				if (mat.has_value())
				{
					m_material = mat.value();
					g_Chams->m_materials.at(m_currectIndex) = m_material;

					saveCfgIndex();

					ImNotify::add(ImNotify{ 5.0f, "Updated material", std::format("Name {} Key {}\nBuf length {}", m_material.data.name, m_material.data.key, m_material.data.buf.length()) });
				}
			}
			else
				ImNotify::add(ImNotify{ 5.0f, "Error updating material", "Can't update material\nInvalid name / key" });
		}

		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_D))
		{
			/*const auto toCopy = m_ImEditor.GetCurrentLineText();
			m_ImEditor.Paste(toCopy);*/
		}

		m_ImEditor.Render("Editor##mat");

		ImGui::EndChild();
	}
}

bool MaterialEditor::isMaterialValid(const Mat_t& mat, bool updating) const
{
	if (!updating)
	{
		for (auto i : std::views::iota(m_oldIndex, g_Chams->m_materials.size()))
		{
			if (g_Chams->m_materials.at(i).data.name == mat.data.name)
				return false;
		}
	}

	return !mat.data.name.empty()
		&& !mat.data.key.empty()
		&& !mat.data.buf.empty();
}

void MaterialEditor::initEditor()
{
	m_folderName = "materials";
	m_saveDir = config.getHackPath() / config.getExtraLoadPath() / m_folderName / getPathForConfig();

	m_oldIndex = g_Chams->m_materials.size();
	m_currectIndex = m_oldIndex;
	loadCfg();

	for (auto i : std::views::iota(m_oldIndex, g_Chams->m_materials.size()))
	{
		auto addCorrectStrategy = [i]()
		{
			return g_Chams->m_materials.at(i).strategy == Mat_t::StrategyType::FROM_STRING
				? g_Chams->addMaterialByString(g_Chams->m_materials.at(i)).value()
				: g_Chams->addMaterialByBuffer(g_Chams->m_materials.at(i)).value();
		};

		g_Chams->m_materials.at(i).mat = addCorrectStrategy().mat;
	}

	if (m_oldIndex == g_Chams->m_materials.size())
		m_material = {};
	else
		m_material = getMaterialIndexed(m_currectIndex).value();

	m_ImEditor.SetText(m_material.data.buf);
	m_ImEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::ValveKeyValues());
	m_ImEdtorSubAdd.SetLanguageDefinition(TextEditor::LanguageDefinition::ValveKeyValues());
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
			Mat_t material;
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
	for (auto i : std::views::iota(m_oldIndex, g_Chams->m_materials.size()))
		to_json(j[g_Chams->m_materials.at(i).data.name], g_Chams->m_materials.at(i));

	if (!j.empty())
		m_json.update(j);

	out << std::setw(4) << m_json;

	return true;
}

bool MaterialEditor::saveCfgIndex()
{
	std::ofstream out{ m_saveDir };
	if (!out)
		return false;

	json j;
	
	to_json(j[g_Chams->m_materials.at(m_currectIndex).data.name], g_Chams->m_materials.at(m_currectIndex));

	if (!j.empty())
		m_json.update(j);

	out << std::setw(4) << m_json;

	return true;
}

std::filesystem::path MaterialEditor::getPathForConfig() const
{
	std::filesystem::path path{ m_folderName };
	if (path.extension() != ".json")
		path.replace_extension(".json");

	return path;
}

std::optional<Mat_t> MaterialEditor::getMaterialIndexed(size_t index) const
{
	if (index < m_oldIndex)
		return std::nullopt;

	const auto& material = g_Chams->m_materials.at(index);
	return Mat_t
	{
		.isFromEditor = true, .type = material.type, .strategy = material.strategy,
		.data = Mat_t::Data{.name = material.data.name, .key = material.data.key, .buf = material.data.buf }
	};
}

std::vector<Mat_t> MaterialEditor::getEditorMaterials() const
{
	std::vector<Mat_t> res{};
	
	for (auto i : std::views::iota(m_oldIndex, g_Chams->m_materials.size()))
		res.push_back(g_Chams->m_materials.at(i));

	return res;
}