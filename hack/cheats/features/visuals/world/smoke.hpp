#pragma once

#include <cheats/classes/renderableToSurface.hpp>
#include <cheats/classes/frameStage.hpp>

#include <SDK/math/Vector.hpp>

class SmokeDraw : protected RenderableSurfaceType
{
public:
	constexpr SmokeDraw() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw() override;
private:
	void drawCustomSmokeEffect(const Vec3& pos, float radius);
};

GLOBAL_FEATURE(SmokeDraw);

class SmokeRemoval : protected FrameStageType
{
public:
	constexpr SmokeRemoval() :
		FrameStageType{}
	{}

protected:
	virtual void run(int frame) override;
};

GLOBAL_FEATURE(SmokeRemoval);
