#include "flashlight.hpp"

#include <SDK/CFlashlightEffect.hpp>
#include <SDK/IMemAlloc.hpp>
#include <SDK/Enums.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/ISurface.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/math/math.hpp>
#include <gamememory/memory.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/tools/tools.hpp>

void Flashlight::init()
{

}

CFlashlightEffect* Flashlight::createFlashlight(float fov, Entity_t* ent, const char* effectName,
	float farZ, float linearAtten)
{
	auto flashlightMemory = reinterpret_cast<CFlashlightEffect*>(interfaces::memAlloc->_alloc(sizeof(CFlashlightEffect)));
	if (!flashlightMemory)
		return nullptr;

	int idx = ent->getIndex(); // allow asm passing this arg
	void* callAddr = g_Memory.m_flashlightCreate();
	__asm
	{
		mov eax, fov
		mov ecx, flashlightMemory
		push linearAtten
		push farZ
		push effectName
		push idx
		call callAddr
	}

	return flashlightMemory;
}

void Flashlight::destroyFlashLight(CFlashlightEffect* flashlight)
{
	g_Memory.m_flashlightDestroy()(flashlight, nullptr); // second arg is not even used there
}

void Flashlight::updateFlashlight(CFlashlightEffect* flashlight, const Vector& pos, const Vector& forward, const Vector& right, const Vector& up)
{
	g_Memory.m_flashlightUpdate()(flashlight, flashlight->m_entIndex, pos, forward, right, up, flashlight->m_fov, flashlight->m_farZ, flashlight->m_LinearAtten, flashlight->m_castsShadows, flashlight->m_textureName);
}

void Flashlight::run(int frame)
{
	if (globals::isShutdown)
		return;

	if (frame != FRAME_RENDER_START)
		return;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	if (!config.get<bool>(vars.bFlashlight))
		return;

	auto key = config.get<Key>(vars.kFlashlight);
	switch (key.getKeyMode())
	{
	case KeyMode::DOWN:
	{
		static bool done = false;

		if (key.isDown())
		{
			if (!done)
			{
				interfaces::surface->playSound(XOR("items\\flashlight1.wav"));
				m_flashlight = createFlashlight(config.get<float>(vars.fFlashlightFov), game::localPlayer());
				done = true;
			}
		}
		else
		{
			if (done)
			{
				destroyFlashLight(m_flashlight);
				m_flashlight = nullptr;
				done = false;
			}
		}

		break;
	}
	case KeyMode::TOGGLE:
	{
		if (key.isPressed())
		{
			interfaces::surface->playSound(XOR("items\\flashlight1.wav"));

			if (m_flashlight)
			{
				destroyFlashLight(m_flashlight);
				m_flashlight = nullptr;
			}
			else
				m_flashlight = createFlashlight(config.get<float>(vars.fFlashlightFov), game::localPlayer());
		}

		break;
	}
	}

	if (!m_flashlight)
		return;

	Vector angle;
	interfaces::engine->getViewAngles(angle);
	auto [forward, right, up] = math::angleVectors(angle);

	m_flashlight->m_isOn = true;
	m_flashlight->m_castsShadows = true;
	m_flashlight->m_bigMode = config.get<bool>(vars.bFlashlightBigMode);
	m_flashlight->m_fov = config.get<float>(vars.fFlashlightFov);

	updateFlashlight(m_flashlight, game::localPlayer->getEyePos(), forward, right, up);
}

void Flashlight::shutdown()
{
	if (m_flashlight)
	{		
		destroyFlashLight(m_flashlight);
		m_flashlight = nullptr;
	}
}