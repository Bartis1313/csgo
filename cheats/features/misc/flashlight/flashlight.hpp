#pragma once

#include <classes/frameStage.hpp>
#include <utilities/tools/tools.hpp>

#include <SDK/math/Vector.hpp>

class Entity_t;
class CFlashlightEffect;

class Flashlight : public FrameStageType
{
public:
	constexpr Flashlight() :
		FrameStageType{}
	{}

	virtual void init();
	virtual void run(int frame);
	virtual void shutdown();
private:
	CFlashlightEffect* createFlashlight(float fov, Entity_t* ent, const char* effectName = XOR("effects/flashlight001"),
		float farZ = 1000.0f, float linearAtten = 1000.0f);
	void destroyFlashLight(CFlashlightEffect* flashlight);
	void updateFlashlight(CFlashlightEffect* flashlight, const Vec3& pos, const Vec3& forward, const Vec3& right, const Vec3& up);

	CFlashlightEffect* m_flashlight = nullptr;
};

[[maybe_unused]] inline auto g_Flashlight = Flashlight{};