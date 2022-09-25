#pragma once

#include <classes/frameStage.hpp>
#include <utilities/tools/tools.hpp>

class Entity_t;
class CFlashlightEffect;
struct Vector;

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
	void updateFlashlight(CFlashlightEffect* flashlight, const Vector& pos, const Vector& forward, const Vector& right, const Vector& up);

	CFlashlightEffect* m_flashlight = nullptr;
};

[[maybe_unused]] inline auto g_Flashlight = Flashlight{};