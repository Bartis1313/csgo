#include "flashlight.hpp"

#include "../../../../../SDK/CFlashlightEffect.hpp"
#include "../../../../../SDK/IMemAlloc.hpp"
#include "../../../../../SDK/Enums.hpp"
#include "../../../../../SDK/IVEngineClient.hpp"
#include "../../../../../SDK/ISurface.hpp"
#include "../../../../../SDK/interfaces/interfaces.hpp"
#include "../../../../../SDK/structs/Entity.hpp"
#include "../../../../../SDK/structs/IDXandPaterrns.hpp"

#include "../../../../game.hpp"
#include "../../../../globals.hpp"
#include "../../../../../config/vars.hpp"
#include "../../../../../utilities/math/math.hpp"

void Flashlight::init()
{
	m_createAddr = utilities::patternScan(CLIENT_DLL, FLASHLIGHT_CREATE);
	m_destroyAddr = reinterpret_cast<destroyType>(utilities::patternScan(CLIENT_DLL, FLASHLIGHT_DESTROY));
	const uintptr_t update = utilities::patternScan(CLIENT_DLL, FLASHLIGHT_UPDATE); // call
	const uintptr_t relativeAddress = *reinterpret_cast<uintptr_t*>(update + 0x1); // read the rel32
	const uintptr_t nextInstruction = update + 0x5;
	m_updateAddr = relativeAddress + nextInstruction;
}

CFlashlightEffect* Flashlight::createFlashlight(float fov, Entity_t* ent, const char* effectName,
	float farZ, float linearAtten)
{
	auto flashlightMemory = reinterpret_cast<CFlashlightEffect*>(interfaces::memAlloc->_alloc(sizeof(CFlashlightEffect)));
	if (!flashlightMemory)
		return nullptr;

	int idx = ent->getIndex(); // allow asm passing this arg
	void* callAddr = reinterpret_cast<void*>(m_createAddr);
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
	using fn = void(__thiscall*)(void*, void*);
	m_destroyAddr(flashlight, nullptr); // second arg is not even used there
}

void Flashlight::updateFlashlight(CFlashlightEffect* flashlight, const Vector& pos, const Vector& forward, const Vector& right, const Vector& up)
{
	using fn = void(__thiscall*)(void*, int, const Vector&, const Vector&, const Vector&, const Vector&, float, float, float, bool, const char*);

	const static auto fun = reinterpret_cast<fn>(m_updateAddr);
	fun(flashlight, flashlight->m_entIndex, pos, forward, right, up, flashlight->m_fov, flashlight->m_farZ, flashlight->m_LinearAtten, flashlight->m_castsShadows, flashlight->m_textureName);
}

void Flashlight::run(int frame)
{
	if (m_flashlight && globals::isShutdown)
	{
		static auto bOnce = [=]()
		{
			destroyFlashLight(m_flashlight);
			m_flashlight = nullptr;

			return true;
		} ();
	}

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