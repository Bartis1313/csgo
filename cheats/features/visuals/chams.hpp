#pragma once

#include <string>
#include <vector>

#include "../../../SDK/IVModelRender.hpp"

class Color;
struct DrawModelState_t;
struct ModelRenderInfo_t;
struct Matrix3x4;
class Player_t;
class IMaterial;
class KeyValues;
struct Matrix3x4;

class Chams final
{
public:
	void init();
	~Chams();
	void run(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix);
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
	Mat_t createFromBuf(const std::string& name, bool ignore, bool wireframe, const std::string& shader, const std::string& baseTexture = "",
		const std::string& envMap = "", const std::string& proxies = "");

	void drawBackTrack(Player_t* ent);
	void CALL(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix);
	Mat_t m_flat;
	Mat_t m_generic;
	Mat_t m_glow;
	Mat_t m_metalic;
	Mat_t m_pearlescent;
	// for mem buffer
	std::vector<KeyValues*> m_keyBuf;
	void* m_result;
	DrawModelState_t m_state;
	ModelRenderInfo_t m_info;
	Matrix3x4* m_matrix;
};

inline Chams chams;