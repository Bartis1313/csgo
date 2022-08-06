#pragma once

#include "../../../classes/onlyInit.hpp"
#include "../../../classes/frameStage.hpp"

class Smoke_t;
struct Vector;
class Color;
struct ImFont;
using ImDrawFlags = int;

class SmokeDraw : public OnlyInitType
{
public:
	constexpr SmokeDraw() :
		OnlyInitType{}
	{}

	virtual void init();
	void draw(Smoke_t* ent);
private:
	void drawArc3DSmoke(const Vector& pos, const float radius, const int points, const float percent,
		const Color& color, const ImDrawFlags flags, const float thickness, ImFont* font, const std::string& text, const Color& colorText);
	void drawCustomSmokeEffect(const Vector& pos, float radius);
};

[[maybe_unused]] inline auto g_SmokeSraw = SmokeDraw{};

class SmokeRemoval : public FrameStageType
{
public:
	constexpr SmokeRemoval() :
		FrameStageType{}
	{}

	virtual void init();
	virtual void run(int frame);
private:
	uintptr_t m_smokeCountAddr;
};

[[maybe_unused]] inline auto g_SmokeRemoval = SmokeRemoval{};
