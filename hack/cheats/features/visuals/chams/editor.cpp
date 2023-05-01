#include "editor.hpp"

#include <utilities/tools/wrappers.hpp>
#include <utilities/tools/tools.hpp>
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

#include <imgui.h>
#include <imgui_internal.h>
#include <ranges>
#include <fstream>
#include <Shellapi.h>

#include <cheats/helper/initable.hpp>

namespace
{
	struct EditorHandler : InitAble
	{
		EditorHandler()
		{
			this->registerInit(materialEditor::initEditor);
		}
	} editorHandler;
}

using json = nlohmann::json;

void from_json(const json& j, MaterialData& val)
{
	from_json(j, "Name", val.name);
	from_json(j, "Key", val.key);
	from_json(j, "Buf", val.buffer);
	from_json(j, "Creation", val.createType);
	from_json(j, "Type", val.materialType);
}

void to_json(json& j, const MaterialData& val)
{
	j["Name"] = val.name;
	j["Key"] = val.key;
	j["Buf"] = val.buffer;
	j["Creation"] = val.createType;
	j["Type"] = val.materialType;
}

void markdownEditor(const std::string& markdown_)
{
	static ImGui::MarkdownConfig mdConfig;

	auto callback = [](ImGui::MarkdownLinkCallbackData clbData)
	{
		std::string url{ clbData.link, static_cast<size_t>(clbData.linkLength) };
		if (!clbData.isImage)
		{
			ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
	};

	mdConfig.linkCallback = callback;
	ImGui::Markdown(markdown_.c_str(), markdown_.length(), mdConfig);
}

void materialEditor::draw()
{
	ImGui::BeginChild("##editor");
	{
		renderAddMaterial();
		renderMaterialsList();
		renderMaterialsView();
	}
	ImGui::EndChild();
}

void materialEditor::renderHelpBar()
{
	const std::string text
	{
		"(First, read basics written [here](https://developer.valvesoftware.com/wiki/KeyValues)\n"
		"Second, read about [materials](https://developer.valvesoftware.com/wiki/Material)\n"
		"**CTRL-S as a shortcut adds / updates current material**)"
	};
	markdownEditor(text);
}

void materialEditor::renderAddMaterial()
{
	if (!m_subAddOpen)
		return;

	if (ImGui::Begin("New material", &m_subAddOpen, ImGuiWindowFlags_NoCollapse))
	{
		renderMaterialInfo(m_materialToAdd);

		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
		{
			// override always on saving attempt only
			m_materialToAdd.buffer = m_ImEdtorSubAdd.GetText();
			if (isMaterialValid(m_materialToAdd))
			{
				// everything provided, now update material
				m_materialToAdd.material = material::factory::createMaterial(m_materialToAdd);
				// update to general view
				editorMaterials.push_back(m_materialToAdd);
				// update to chams too, because it's where whole texture render goes
				chams::materials.push_back(m_materialToAdd);

				// force new material to be selected in view child
				index = editorMaterials.size() - 1;
				// force new material buffer to be displayed in view
				m_ImEditor.SetText(m_materialToAdd.buffer);

				saveCfgIndex();

				ImNotify::add(ImNotify{ 5.0f, "Added material", std::format("Name {} Key {}\nBuf length {}", m_materialToAdd.name, m_materialToAdd.key, m_materialToAdd.buffer.length()) });

				m_subAddOpen = false;
				// after close, we could reset materialtoadd, I prefer to cache last added
			}
			else
				ImNotify::add(ImNotify{ 5.0f, "Error adding material", "Please check duplication or you left inputs empty" });
		}

		m_ImEdtorSubAdd.Render("Material to add");

		ImGui::End();
	}
}

void materialEditor::renderMaterialInfo(MaterialData& mat)
{
	ImGui::TextUnformatted("Strategy");
	ImGui::RadioButton("Buffer", reinterpret_cast<int*>(&mat.createType), 0);
	ImGui::SameLine();
	ImGui::RadioButton("From String", reinterpret_cast<int*>(&mat.createType), 1);
	ImGui::InputText("Name", &mat.name);
	ImGui::InputText("Key", &mat.key);
	ImGui::Combo("Style", reinterpret_cast<int*>(&mat.materialType), magic_enum::enum_names_pretty<MaterialTypes>());
}

void materialEditor::renderMaterialsList()
{
	ImGui::BeginChild("left pane", ImVec2{ 150.0f, 0.0f }, true);
	{
		for (size_t i = 0; i < editorMaterials.size(); ++i)
		{
			if (ImGui::Selectable(editorMaterials.at(i).name.c_str(), index == i))
			{
				// cache text
				editorMaterials.at(index).buffer = m_ImEditor.GetText();
				index = i;
				m_ImEditor.SetText(editorMaterials.at(index).buffer);
			}
		}

		ImGui::EndChild();
	}
	ImGui::SameLine();
}

void materialEditor::renderMaterialsView()
{
	ImGui::BeginChild("Materials view");
	{

		ImGui::BeginChild("InfoL", ImVec2{ ImGui::GetContentRegionAvail().x * 0.5f, 50.0f }, false);
		{
			renderHelpBar();

			ImGui::EndChild();
		}

		ImGui::SameLine();

		ImGui::BeginChild("InfoR", ImVec2{ 0.0f, 50.0f }, false);
		{
			if (ImGui::Button("Press to open Add-Window##material"))
			{
				m_subAddOpen = true;
			}

			if (ImGui::Button("Delete"))
			{
				if (!editorMaterials.empty())
				{
					const auto& backupName = editorMaterials.at(index).name;
					m_json.erase(m_json.find(editorMaterials.at(index).name));
					chams::materials.erase(chams::materials.end() - editorMaterials.size() + index);
					editorMaterials.erase(editorMaterials.begin() + index);
					index -= 1;

					saveCfg();

					if (editorMaterials.empty())
						m_ImEditor.SetText("");
					else
						m_ImEditor.SetText(editorMaterials.at(index).buffer);

					ImNotify::add(ImNotify{ 5.0f, "Deleted material", std::format("Name {}", backupName) });
				}
				else
					ImNotify::add(ImNotify{ 5.0f, "Deleted nothing", "You have 0 materials..." });
			}

			ImGui::EndChild();
		}


		if (!editorMaterials.empty())
			ImGui::TextUnformatted(std::format("{}", editorMaterials.at(index).name).c_str());
		else
			ImGui::TextUnformatted("No materials yet, add one!");

		if (editorMaterials.size())
		{
			ImGui::Separator();

			auto& materialNow = editorMaterials.at(index);

			renderMaterialInfo(materialNow);

			// we update the material that already exists.
			if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
			{
				if (isMaterialValid(materialNow, true))
				{
					// overwrite material
					materialNow.material = material::factory::createMaterial(materialNow);
					saveCfgIndex();

					ImNotify::add(ImNotify{ 5.0f, "Updated material", std::format("Name {} Key {}\nBuf length {}", materialNow.name, materialNow.key, materialNow.buffer.length()) });
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
		}

		ImGui::EndChild();
	}
}

bool materialEditor::isMaterialValid(MaterialData& _data, bool updating)
{
	auto allMaterials = editorMaterials;
	// special case: we update material and want to keep detection, but add current record as exception
	if (updating)
		allMaterials.erase(allMaterials.begin() + index);

	const auto itr = std::ranges::find_if(allMaterials, [_data](const MaterialData& data) { return data.name == _data.name; });
	if (itr != allMaterials.end())
		return false;
	
	return !_data.name.empty()
		&& !_data.key.empty();
}

void materialEditor::initEditor()
{
	m_folderName = "materials";
	m_saveDir = config.getHackPath() / config.getExtraLoadPath() / m_folderName / getPathForConfig();

	loadCfg();

	m_ImEditor.SetText(editorMaterials.empty() ? "" : editorMaterials.front().buffer);
	m_ImEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::ValveKeyValues());
	m_ImEdtorSubAdd.SetLanguageDefinition(TextEditor::LanguageDefinition::ValveKeyValues());
}

bool materialEditor::loadCfg()
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
			MaterialData material;
			from_json(value, material);

			editorMaterials.push_back(material);
			// now also inform chams to add those
			chams::materials.push_back(material);
		}
	}

	return true;
}

bool materialEditor::saveCfg()
{
	std::ofstream out{ m_saveDir };
	if (!out)
		return false;

	json j;

	for (auto& material : editorMaterials)
		to_json(j[material.name], material);

	if (!j.empty())
		m_json.update(j);

	out << std::setw(4) << m_json;

	return true;
}

bool materialEditor::saveCfgIndex()
{
	std::ofstream out{ m_saveDir };
	if (!out)
		return false;

	json j;
	
	to_json(j[editorMaterials.at(index).name], editorMaterials.at(index));

	if (!j.empty())
		m_json.update(j);

	out << std::setw(4) << m_json;

	return true;
}

std::filesystem::path materialEditor::getPathForConfig()
{
	std::filesystem::path path{ m_folderName };
	if (path.extension() != ".json")
		path.replace_extension(".json");

	return path;
}