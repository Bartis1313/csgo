#pragma once

#include <classes/drawModel.hpp>
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

	struct Mat_t
	{
		struct Data
		{
			std::string name{};
			std::string key{};
			std::string buf{};
		};

		enum class ExtraType
		{
			NONE,
			GLOW = 1
		};

		bool isFromEditor{ false };
		Data data{};
		IMaterial* mat{};
		ExtraType type{ ExtraType::NONE };

		constexpr IMaterial* operator->() const { return mat; }
		constexpr bool operator!() const { return mat == nullptr; }
	};
	std::vector<Mat_t> getMaterials() const { return m_materials;  }
protected:
	virtual void run(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix) override;
	virtual void init() override;
private:
	void overrideChams(int styles, bool ignore, bool wireframe, const Color& color, bool force = true, bool call = true);
	void drawBackTrack(Player_t* ent);
	void CALL(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix);

	std::optional<Mat_t> addMaterialByBuffer(const Mat_t& material, bool suppress = false);
	std::optional<Mat_t> addMaterialByString(const Mat_t& material, bool suppress = false);

	std::vector<Mat_t> m_materials;

	void* m_result;
	DrawModelState_t m_state;
	ModelRenderInfo_t m_info;
	Matrix3x4* m_matrix;

	friend MaterialEditor;
};

GLOBAL_FEATURE(Chams);