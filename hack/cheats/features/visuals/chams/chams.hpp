#pragma once

#include "mat.hpp"

#include <cheats/classes/drawModel.hpp>
#include <SDK/IVModelRender.hpp>
#include <SDK/IVModelRender.hpp>
#include <SDK/math/matrix.hpp>

#include <string>
#include <vector>
//#include <propagate_const> impossible rn

class Color;
struct DrawModelState_t;
struct ModelRenderInfo_t;
class Player_t;
class IMaterial;
class KeyValues;

class MaterialEditor;

class Chams : protected DrawModelType
{
public:
	Chams() :
		DrawModelType{}
	{}

	std::vector<Mat_t> getMaterials() const { return m_materials; }
	void CALL(Matrix3x4* matrix);
protected:
	virtual void run(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix) override;
private:
	virtual void initMaterials();

	void overrideChams(int styles, bool ignore, bool wireframe, const Color& color, bool force = true, bool call = true);
	void drawBackTrack(Player_t* ent);

	std::optional<Mat_t> addMaterialByBuffer(const Mat_t& material);
	std::optional<Mat_t> addMaterialByString(const Mat_t& material);

	std::vector<Mat_t> m_materials;

	void* m_result;
	DrawModelState_t m_state;
	ModelRenderInfo_t m_info;
	Matrix3x4* m_matrix;

	friend MaterialEditor;
};

GLOBAL_FEATURE(Chams);