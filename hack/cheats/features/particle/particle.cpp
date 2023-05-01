#include "particle.hpp"

#include <SDK/interfaces/interfaces.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/CEffectData.hpp>
#include <utilities/utilities.hpp>
#include <gamememory/memory.hpp>

#if PARTICLE_MANUAL == true
void* Particle::getCallAddr(const std::string& name)
{
	int ret = -1;
	if (memory::particleIsCached()(memory::particleSystem(), name.c_str()))
		memory::particleFindStringIndex()(reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(memory::particleSystem()) + 0x14), &ret, name.c_str()); // esi+14h

	if (ret == -1)
		return nullptr;
	// *v3 + 4 * v5 - > v5 is casted unsigned __int16
	return *reinterpret_cast<void**>(*reinterpret_cast<uintptr_t*>(memory::particleSystem()) + (0x4 * static_cast<uint16_t>(ret)));
}

void Particle::dispatchParticle(const std::string& name, const Vec3& pos)
{
	Vec3 copyPos = pos;

	void* addr = getCallAddr(name);
	const static auto _call = memory::particleCall();
	void* created = createParticle(_call, addr, &copyPos);

	// update pos, you can see bytes in set control point
	__asm add esp, 8
	*reinterpret_cast<Vec3*>(reinterpret_cast<uintptr_t>(created) + 0x3B4) = copyPos;

	// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/client/c_particle_system.cpp#L339
	memory::particleSetControlPoint()(created, 0, &copyPos);
	memory::particleSetControlPoint()(created, 1, &copyPos);
}

// because calling ParticleStart or doing this by DispatchEvent did not work
// have to wrap this for new particle keyvalues method, new because judging by xrefs there was some "old" method
__declspec(naked) void* Particle::createParticle(void* caller, void* addr, Vec3* pos)
{
	__asm
	{
		push ebp
		mov ebp, esp
		push 0x0FFFFFFFF
		push 0
		mov edx, addr
		push ecx
		push pos
		call caller
		add esp, 0x10
		mov esp, ebp
		pop ebp
		retn
	}
}
#else

void particle::dispatchParticleEffect(const std::string_view name, const Vec3& pos, const Vec3& angles)
{
	memory::dispatchParticleEffect()(name.data(), pos, angles, nullptr, -1, nullptr);
}

void particle::stopParticleEffect(const std::string_view name)
{
	CEffectData data{ };

	// we skip entity index handling

	data.m_hitBox = memory::getParticleSystemIndex()(name.data());

	memory::dispatchEffect()("ParticleEffectStop", data);
}

#endif