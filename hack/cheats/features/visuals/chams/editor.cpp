#include "editor.hpp"

#include "factory/factory.hpp"
#include "chams.hpp"

#include <utilities/tools/wrappers.hpp>
#include <utilities/tools/tools.hpp>
#include <menu/GUI-ImGui/imguiaddons.hpp>
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
#include <imgui_stdlib.h>
#include <ranges>
#include <fstream>
#include <Shellapi.h>
#include <nlohmann/json.hpp>
#include <deps/ImGui/editor/TextEditor.hpp>

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

namespace materialEditor
{
	bool loadCfg();
	bool saveCfg();
	bool saveCfgIndex();

	bool isMaterialValid(MaterialData& data, bool updating = false);
	std::filesystem::path getPathForConfig();

	void renderHelpBar();
	void renderAddMaterial();
	void renderMaterialsList();
	void renderMaterialsView();
	void renderMaterialInfo(MaterialData& mat);

	std::vector<MaterialData> editorMaterials;
	size_t index{ 0 };

	bool opened{ false };
	bool subAddOpened{ false };

	constexpr std::string_view folderName{ "materials" };
	std::filesystem::path saveDir;

	TextEditor mainEditor;
	TextEditor subAddEditor;

	MaterialData materialToAdd;
	nlohmann::json m_json;
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
		"First, read basics written [here](https://developer.valvesoftware.com/wiki/KeyValues)\n"
		"Second, read about [materials](https://developer.valvesoftware.com/wiki/Material)\n"
		"**CTRL-S as a shortcut adds / updates current material**"
	};
	markdownEditor(text);
}

void materialEditor::renderAddMaterial()
{
	if (!subAddOpened)
		return;

	if (ImGui::Begin("New material", &subAddOpened, ImGuiWindowFlags_NoCollapse))
	{
		renderMaterialInfo(materialToAdd);

		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
		{
			// override always on saving attempt only
			materialToAdd.buffer = subAddEditor.GetText();
			if (isMaterialValid(materialToAdd))
			{
				// everything provided, now update material
				materialToAdd.material = material::factory::createMaterial(materialToAdd);
				// update to general view
				editorMaterials.push_back(materialToAdd);
				// update to chams too, because it's where whole texture render goes
				chams::materials.push_back(materialToAdd);

				// force new material to be selected in view child
				index = editorMaterials.size() - 1;
				// force new material buffer to be displayed in view
				mainEditor.SetText(materialToAdd.buffer);

				saveCfgIndex();

				ImNotify::add(ImNotify{ 5.0f, "Added material", std::format("Name {} Key {}\nBuf length {}", materialToAdd.name, materialToAdd.key, materialToAdd.buffer.length()) });

				subAddOpened = false;
				// after close, we could reset materialtoadd, I prefer to cache last added
			}
			else
				ImNotify::add(ImNotify{ 5.0f, "Error adding material", "Please check duplication or you left inputs empty" });
		}

		subAddEditor.Render("Material to add");

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
				editorMaterials.at(index).buffer = mainEditor.GetText();
				index = i;
				mainEditor.SetText(editorMaterials.at(index).buffer);
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
				subAddOpened = true;
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
						mainEditor.SetText("");
					else
						mainEditor.SetText(editorMaterials.at(index).buffer);

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

			mainEditor.Render("Editor##mat");
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
	saveDir = config::getHackPath() / config::getExtraLoadPath() / folderName / getPathForConfig();

	loadCfg();

	mainEditor.SetText(editorMaterials.empty() ? "" : editorMaterials.front().buffer);
	mainEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::ValveKeyValues());
	subAddEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::ValveKeyValues());
}

bool materialEditor::loadCfg()
{
	if (auto path = config::getHackPath() / config::getExtraLoadPath() / folderName; !std::filesystem::exists(path))
		std::filesystem::create_directories(path);

	std::ifstream input{ saveDir };
	if (!input)
		return false;

	if (!std::filesystem::is_empty(saveDir))
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
	std::ofstream out{ saveDir };
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
	std::ofstream out{ saveDir };
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
	std::filesystem::path path{ folderName };
	if (path.extension() != ".json")
		path.replace_extension(".json");

	return path;
}