#pragma once

#include <cheats/classes/frameStage.hpp>
#include <utilities/tools/tools.hpp>

#include <SDK/math/Vector.hpp>

class Entity_t;
class CFlashlightEffect;

class Flashlight : protected FrameStageType
{
public:
	constexpr Flashlight() :
		FrameStageType{}
	{}

protected:
	virtual void run(int frame) override;
	virtual void shutdown() override;
private:
	CFlashlightEffect* createFlashlight(float fov, Entity_t* ent, const char* effectName = "effects/flashlight001",
		float farZ = 1000.0f, float linearAtten = 1000.0f);
	void destroyFlashLight(CFlashlightEffect* flashlight);
	void updateFlashlight(CFlashlightEffect* flashlight, const Vec3& pos, const Vec3& forward, const Vec3& right, const Vec3& up);

	CFlashlightEffect* m_flashlight = nullptr;
};

GLOBAL_FEATURE(Flashlight);