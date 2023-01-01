#pragma once

#include "chams.hpp"

#include <classes/renderableToPresent.hpp>
#include <dependencies/json.hpp>
#include <dependencies/ImGui/TextEditor.h>

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
private:

	bool loadCfg();
	bool saveCfg();
	std::filesystem::path getPathForConfig() const;
	std::optional<Chams::Mat_t> getMaterialIndexed(size_t index) const;

	bool m_open{ false };
	size_t m_oldIndex; // before custom
	std::string_view m_folderName;
	std::filesystem::path m_saveDir;
	TextEditor m_ImEditor;
	nlohmann::json m_json;
};

GLOBAL_FEATURE(MaterialEditor);