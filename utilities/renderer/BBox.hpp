#pragma once
#pragma warning(disable:26495)

#include <SDK/math/Vector.hpp>
#include <dependencies/ImGui/imgui.h>

#include <array>
#include <utility>

class Entity_t;

struct Box final
{
	Box() = default;
	constexpr Box(float x, float y, float w, float h) :
		x{ x }, y{ y }, w{ w }, h{ h }
	{}
	constexpr Box(const std::array<ImVec2, 8>& arr) :
		points{ arr }
	{}
	Box(Entity_t* ent);

	constexpr bool isValid() const { return m_isValid; }

	std::array<ImVec2, 8> points;
	float x, y, w, h;
	ImVec2 topleft;
	ImVec2 topright;
	ImVec2 bottomleft;
	ImVec2 bottomright;
private:
	std::array<Vec3, 8> buildAABB(const Vec3& min, const Vec3& max);
	bool m_isValid;
};