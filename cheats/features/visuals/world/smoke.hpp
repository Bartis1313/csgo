#pragma once

#include <classes/renderableToSurface.hpp>
#include <classes/frameStage.hpp>

#include <SDK/math/Vector.hpp>

class Color;
struct ImFont;
using ImDrawFlags = int;

class SmokeDraw : public RenderableSurfaceType
{
public:
	constexpr SmokeDraw() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
private:
	void drawCustomSmokeEffect(const Vec3& pos, float radius);
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
};

[[maybe_unused]] inline auto g_SmokeRemoval = SmokeRemoval{};
