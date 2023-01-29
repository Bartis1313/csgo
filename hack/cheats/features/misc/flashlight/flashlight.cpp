#include "flashlight.hpp"

#include <SDK/CFlashlightEffect.hpp>
#include <SDK/IMemAlloc.hpp>
#include <SDK/Enums.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/ISurface.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/math/math.hpp>
#include <gamememory/memory.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/tools/tools.hpp>

CFlashlightEffect* Flashlight::createFlashlight(float fov, Entity_t* ent, const char* effectName,
	float farZ, float linearAtten)
{
	auto flashlightMemory = reinterpret_cast<CFlashlightEffect*>(memory::interfaces::memAlloc->_alloc(sizeof(CFlashlightEffect)));
	if (!flashlightMemory)
		return nullptr;

	memory::flashlightCreate()(flashlightMemory, nullptr, 0.0f, 0.0f, 0.0f, fov, ent->getIndex(), effectName, farZ, linearAtten);

	return flashlightMemory;
}

void Flashlight::destroyFlashLight(CFlashlightEffect* flashlight)
{
	memory::flashlightDestroy()(flashlight, nullptr); // second arg is not even used there
}

void Flashlight::updateFlashlight(CFlashlightEffect* flashlight, const Vec3& pos, const Vec3& forward, const Vec3& right, const Vec3& up)
{
	memory::flashlightUpdate()(flashlight, flashlight->m_entIndex, pos, forward, right, up, flashlight->m_fov,
		flashlight->m_farZ, flashlight->m_LinearAtten, flashlight->m_castsShadows, flashlight->m_textureName);
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

	if (!vars::misc->flashLight->enabled)
		return;

	auto key = vars::keys->flashLight;
	switch (key.getKeyMode())
	{
	case KeyMode::DOWN:
	{
		static bool done = false;

		if (key.isDown())
		{
			if (!done)
			{
				memory::interfaces::surface->playSound("items\\flashlight1.wav");
				m_flashlight = createFlashlight(vars::misc->flashLight->fov, game::localPlayer());
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
			memory::interfaces::surface->playSound("items\\flashlight1.wav");

			if (m_flashlight)
			{
				destroyFlashLight(m_flashlight);
				m_flashlight = nullptr;
			}
			else
				m_flashlight = createFlashlight(vars::misc->flashLight->fov, game::localPlayer());
		}

		break;
	}
	}

	if (!m_flashlight)
		return;

	Vec3 angle;
	memory::interfaces::engine->getViewAngles(angle);
	auto [forward, right, up] = math::angleVectors(angle);

	m_flashlight->m_isOn = true;
	m_flashlight->m_castsShadows = true;
	m_flashlight->m_bigMode = vars::misc->flashLight->bigMode;
	m_flashlight->m_fov = vars::misc->flashLight->fov;

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