#pragma once

#include <classes/drawModel.hpp>
#include <SDK/IVModelRender.hpp>
#include <SDK/IVModelRender.hpp>
#include <SDK/math/matrix.hpp>

#include <string>
#include <vector>

class Color;
struct DrawModelState_t;
struct ModelRenderInfo_t;
class Player_t;
class IMaterial;
class KeyValues;

class Chams : protected DrawModelType
{
public:
	Chams() :
		DrawModelType{}
	{}

protected:
	virtual void run(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix) override;
	virtual void init() override;
private:
	void overrideChams(int styles, bool ignore, bool wireframe, const Color& color, bool force = true, bool call = true);
	class Mat_t
	{
	public:
		Mat_t() = default;
		constexpr Mat_t(IMaterial* mat, bool glow = false)
			: m_mat{ mat }, m_glow{ glow }
		{}
		constexpr IMaterial* getMat() const { return m_mat; }
		constexpr bool isGlow() const { return m_glow; }
	private:
		IMaterial* m_mat = nullptr;
		bool m_glow = false;
	};

	void drawBackTrack(Player_t* ent);
	void CALL(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix);
	Mat_t m_flat;
	Mat_t m_generic;
	Mat_t m_glow;
	Mat_t m_metalic;
	Mat_t m_pearlescent;

	void* m_result;
	DrawModelState_t m_state;
	ModelRenderInfo_t m_info;
	Matrix3x4* m_matrix;
};

GLOBAL_FEATURE(Chams);