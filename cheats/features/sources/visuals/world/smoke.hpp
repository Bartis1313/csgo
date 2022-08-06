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
	SmokeDraw()
	{
		m_hacks.push_back(this);
	}

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
	SmokeRemoval()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void run(int frame);
private:
	uintptr_t m_smokeCountAddr;
};

[[maybe_unused]] inline auto g_SmokeRemoval = SmokeRemoval{};
