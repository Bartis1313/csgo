#include "callbacks.hpp"

#include <SDK/CEffectData.hpp>
#include <SDK/CClientEffectRegistration.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/console/console.hpp>
#include <gamememory/memory.hpp>

#include <cheats/helper/initable.hpp>
#include <cheats/helper/shutdownable.hpp>

#include <ranges>
#include <vector>
#include <string>

namespace
{
	/*struct InitCallback : InitAble
	{
		InitCallback()
		{
			this->registerInit(callbacks::init);
		}
	} clInit;

	struct ShutdownCallback : ShutdownAble
	{
		ShutdownCallback()
		{
			this->registerShutdown(callbacks::shutdown);
		}
	} clShutdown;*/
}

namespace callbacks
{
	struct clbStruct
	{
		std::string m_name; // effect's name
		clientEffectCallback m_function = nullptr; // init this on add as custom func
		clientEffectCallback m_callback = nullptr; // will be later inited
	};

	clientEffectCallback getCallbackByMemory(/*const */clientEffectCallback/*&*/ callb);
	void addCallBack(const clbStruct& callb);

	std::vector<clbStruct> callbacks;
	CClientEffectRegistration* head;
}

void callbacks::addCallBack(const clbStruct& callb)
{
	auto itr = std::ranges::find_if(callbacks, [name = callb.m_name](const clbStruct& el)
		{
			return el.m_name == name;
		});

	if (itr != callbacks.cend())
		console::warn("{} already exists! skipping...", callb.m_name);
	else
		callbacks.push_back(callb);
}

// custom1
void example(const CEffectData& data)
{
	auto call = callbacks::getCallbackByMemory(&example);
	call(data);
}

// custom2 - fail
void example2(const CEffectData& data)
{
	auto call = callbacks::getCallbackByMemory(&example2);
	call(data);
}

void callbacks::init()
{
	addCallBack({ "Impact", &example, nullptr });
	addCallBack({ "Impact", &example2, nullptr });

	head = memory::callbacksHead();
	while(head = head->m_next)
	{
		for (auto& el : callbacks)
		{
			// check by names on init, somehow we gotta point to correct name we passed
			if (std::string_view name = head->m_effectName; name == el.m_name)
			{
				el.m_callback = head->m_function; // own now the original function
				head->m_function = el.m_function; // point our edited function

				console::debug("custom function inited, name: {} at addr {:#0x}",
					name, reinterpret_cast<uintptr_t>(el.m_function));
			}
		}
	}
}

void callbacks::shutdown()
{
	head = memory::callbacksHead();
	while (head = head->m_next)
	{
		for (auto& el : callbacks)
		{
			if (el.m_function == head->m_function) // check by address
				head->m_function = el.m_callback; // do not point to custom funcs anymore
		}
	}
}

clientEffectCallback callbacks::getCallbackByMemory(/*const */clientEffectCallback/*&*/ callb)
{
	for (const auto& el : callbacks)
	{
		if (el.m_function == callb)
			return el.m_callback;
	}

	return nullptr;
}

#pragma region dumped_effect_names
/*
ManhackSparks
TeslaZap
TeslaHitboxes
CommandPointer
GunshipImpact
Smoke
MuzzleFlash
Error
BoltImpact
RPGShotDown
GlassImpact
EjectBrass_338Mag
EjectBrass_762Nato
EjectBrass_556
EjectBrass_57
EjectBrass_12Gauge
EjectBrass_9mm
CS_MuzzleFlash_X
CS_MuzzleFlash
KnifeSlash
Impact
CS_HolidayLight
csblood
waterripple
gunshotsplash
watersplashquiet
watersplash
TracerSound
ParticleTracer
Tracer
ShotgunShellEject
RifleShellEject
ShellEject
RagdollImpact
HelicopterMegaBomb
WaterSurfaceExplosion
Explosion
HunterDamage
BloodImpact
bloodspray
WheelDust
ShakeRopes
ParticleEffectStop
ParticleEffect
*/
#pragma endregion
