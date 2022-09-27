#pragma once
#pragma warning(disable:26495)

#include <dependencies/ImGui/imgui.h>

#include <array>
#include <utility>

struct Vector;
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
	std::array<Vector, 8> buildAABB(const Vector& min, const Vector& max);
	Box transformPoints(Entity_t* ent);
	bool m_isValid;
};