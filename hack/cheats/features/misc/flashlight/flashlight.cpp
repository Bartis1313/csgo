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

#include <cheats/hooks/frameStageNotify.hpp>
#include <cheats/hooks/wndproc.hpp>

namespace
{
	struct FlashlightFSN : hooks::FrameStageNotify
	{
		FlashlightFSN()
		{
			this->registerRun(flashlight::run);
			this->registerShutdown(flashlight::shutdown);
		}
	} flashlightFSN;

	struct FlashlightKeys : hooks::wndProcSys
	{
		FlashlightKeys()
		{
			this->registerRun(flashlight::updateKeys);
		}
	};
}

namespace flashlight
{
	CFlashlightEffect* createFlashlight(float fov, Entity_t* ent, float farZ = 1000.0f, float linearAtten = 1000.0f);
	void destroyFlashLight(CFlashlightEffect* flashlight);
	void updateFlashlight(CFlashlightEffect* flashlight, const Vec3& pos, const Vec3& forward, const Vec3& right, const Vec3& up);

	CFlashlightEffect* flashlightPtr{ nullptr };
	const std::string_view flashlightEffect{ "effects/flashlight001" };
	const std::string_view flashlightSound{ "items\\flashlight1.wav" };
}

#pragma optimize("", off)
void flashlight::run(FrameStage stage)
{
	if (globals::isShutdown)
		return;

	if (stage != FRAME_RENDER_START)
		return;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	if (!vars::misc->flashLight->enabled)
		return;

	const auto key = vars::keys->flashLight;
	switch (key.getKeyMode())
	{
	case KeyMode::DOWN:
	{
		static bool done = false;

		if (key.isDown())
		{
			if (!done)
			{
				memory::interfaces::surface->playSound(flashlightSound.data());
				flashlightPtr = createFlashlight(vars::misc->flashLight->fov, game::localPlayer());
				done = true;
			}
		}
		else
		{
			if (done)
			{
				destroyFlashLight(flashlightPtr);
				flashlightPtr = nullptr;
				done = false;
			}
		}

		break;
	}
	case KeyMode::TOGGLE:
	{
		if (key.isPressed())
		{
			memory::interfaces::surface->playSound(flashlightSound.data());

			if (flashlightPtr)
			{
				destroyFlashLight(flashlightPtr);
				flashlightPtr = nullptr;
			}
			else
				flashlightPtr = createFlashlight(vars::misc->flashLight->fov, game::localPlayer());
		}

		break;
	}
	}

	if (!flashlightPtr)
		return;

	const auto [forward, right, up] = math::angleVectors(game::getViewAngles());

	flashlightPtr->m_isOn = true;
	flashlightPtr->m_castsShadows = true;
	flashlightPtr->m_bigMode = vars::misc->flashLight->bigMode;
	flashlightPtr->m_fov = vars::misc->flashLight->fov;

	updateFlashlight(flashlightPtr, game::localPlayer->getEyePos(), forward, right, up);
}
#pragma optimize("", on)

#pragma optimize("", off)
CFlashlightEffect* flashlight::createFlashlight(float fov, Entity_t* ent, float farZ, float linearAtten)
{
	const auto flashlightMemory = reinterpret_cast<CFlashlightEffect*>(memory::interfaces::memAlloc->_alloc(sizeof(CFlashlightEffect)));
	if (!flashlightMemory)
		return nullptr;

	memory::flashlightCreate()(flashlightMemory, nullptr, 0.0f, 0.0f, 0.0f, fov, ent->getIndex(), flashlightEffect.data(), farZ, linearAtten);

	return flashlightMemory;
}
#pragma optimize("", on)

#pragma optimize("", off)
void flashlight::destroyFlashLight(CFlashlightEffect* flashlight)
{
	memory::flashlightDestroy()(flashlight, nullptr); // second arg is not even used there
}
#pragma optimize("", on)

#pragma optimize("", off)
void flashlight::updateFlashlight(CFlashlightEffect* flashlight, const Vec3& pos, const Vec3& forward, const Vec3& right, const Vec3& up)
{
	memory::flashlightUpdate()(flashlight, flashlight->m_entIndex, pos, forward, right, up, flashlight->m_fov,
		flashlight->m_farZ, flashlight->m_LinearAtten, flashlight->m_castsShadows, flashlight->m_textureName);
}
#pragma optimize("", on)

#pragma optimize("", off)
void flashlight::shutdown()
{
	if (flashlightPtr)
	{
		destroyFlashLight(flashlightPtr);
		flashlightPtr = nullptr;
	}
}
#pragma optimize("", on)

void flashlight::updateKeys()
{
	vars::keys->flashLight.update();
}