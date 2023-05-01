#pragma once

#include "chams.hpp"

#include <nlohmann/json.hpp>
#include <deps/ImGui/editor/TextEditor.hpp>


namespace materialEditor
{
	void draw();
	void initEditor();

	bool loadCfg();
	bool saveCfg();
	bool saveCfgIndex();

	[[nodiscard]] bool isMaterialValid(MaterialData& data, bool updating = false);
	std::filesystem::path getPathForConfig();

	void renderHelpBar();
	void renderAddMaterial();
	void renderMaterialsList();
	void renderMaterialsView();
	void renderMaterialInfo(MaterialData& mat);


	inline std::vector<MaterialData> editorMaterials;
	inline size_t index{ 0 };

	inline bool m_open{ false };
	inline bool m_subAddOpen{ false };
	
	inline std::string_view m_folderName;
	inline std::filesystem::path m_saveDir;

	inline TextEditor m_ImEditor;
	inline TextEditor m_ImEdtorSubAdd;

	inline MaterialData m_materialToAdd;
	inline nlohmann::json m_json;
}