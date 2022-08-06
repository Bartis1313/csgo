#pragma once

#include "../../../classes/frameStage.hpp"

#include "../../../../../utilities/utilities.hpp"

class Entity_t;
class CFlashlightEffect;
struct Vector;

class Flashlight : public FrameStageType
{
public:
	Flashlight()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void run(int frame);
private:
	CFlashlightEffect* createFlashlight(float fov, Entity_t* ent, const char* effectName = XOR("effects/flashlight001"),
		float farZ = 1000.0f, float linearAtten = 1000.0f);
	void destroyFlashLight(CFlashlightEffect* flashlight);
	void updateFlashlight(CFlashlightEffect* flashlight, const Vector& pos, const Vector& forward, const Vector& right, const Vector& up);

	uintptr_t m_createAddr;
	using destroyType = void(__thiscall*)(void*, void*);
	destroyType m_destroyAddr;	
	uintptr_t m_updateAddr;
	CFlashlightEffect* m_flashlight = nullptr;
};

[[maybe_unused]] inline auto g_Flashlight = Flashlight{};