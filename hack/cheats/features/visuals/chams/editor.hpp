#pragma once

#include "chams.hpp"

#include <cheats/classes/renderableToPresent.hpp>
#include <nlohmann/json.hpp>
#include <deps/ImGui/editor/TextEditor.hpp>

// this code is a mess. It's a MUST to not friend the Chams
class MaterialEditor : protected RenderablePresentType
{
public:
	MaterialEditor()
		: RenderablePresentType{}
	{}

protected:
	virtual void draw() override;
public:
	void initEditor();
	void changeState() { m_open = !m_open; }
	[[nodiscard]] std::vector<Mat_t> getEditorMaterials() const;
private:

	bool loadCfg();
	bool saveCfg();
	bool saveCfgIndex();
	// doesn't check the buffer from editor! just names to prevent UB
	[[nodiscard]] bool isMaterialValid(const Mat_t& mat, bool updating) const;
	std::filesystem::path getPathForConfig() const;
	std::optional<Mat_t> getMaterialIndexed(size_t index) const;

	void renderHelpBar();
	void renderAddMaterial();
	void renderMaterialsList();
	void renderMaterialsView();
	void renderMaterialInfo(Mat_t& mat);

	bool m_open{ false };
	bool m_subAddOpen{ false };
	size_t m_oldIndex; // before custom
	size_t m_currectIndex; // old index aka loaded chams + now selection
	std::string_view m_folderName;
	std::filesystem::path m_saveDir;
	TextEditor m_ImEditor;
	TextEditor m_ImEdtorSubAdd;
	Mat_t m_material;
	Mat_t m_materialToAdd;
	nlohmann::json m_json;
};

GLOBAL_FEATURE(MaterialEditor);