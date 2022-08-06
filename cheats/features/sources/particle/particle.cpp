#include "particle.hpp"

#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../SDK/math/Vector.hpp"
#include "../../../SDK/structs/IDXandPaterrns.hpp"

#include "../../../utilities/utilities.hpp"

void Particle::init()
{
	m_isCachedAddr = utilities::patternScan(CLIENT_DLL, IS_EFFECT_CACHED);
	m_particleSystenAddr = utilities::patternScan(CLIENT_DLL, PARTICLE_SYSTEM, 0x7);
	m_findStringIndexAddr = utilities::patternScan(CLIENT_DLL, FIND_STRING_INDEX);
	m_callAddr = utilities::patternScan(CLIENT_DLL, PARTICLE_CALL);
}

void* Particle::getCallAddr(const std::string& name)
{
	using cachedfn = bool(__thiscall*)(void*, const char*);
	const static auto isCached = reinterpret_cast<cachedfn>(m_isCachedAddr);
	const static auto particleSystem = **reinterpret_cast<void***>(m_particleSystenAddr);
	using findStringIdxfn = void(__thiscall*)(void*, int*, const char*);
	const static auto findStringIndex = reinterpret_cast<findStringIdxfn>(m_findStringIndexAddr);

	int ret = -1;
	if (isCached(particleSystem, name.c_str()))
		findStringIndex(reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(particleSystem) + 0x14), &ret, name.c_str()); // esi+14h

	if (ret == -1)
		return nullptr;
	// *v3 + 4 * v5 - > v5 is casted unsigned __int16
	return *reinterpret_cast<void**>(*reinterpret_cast<uintptr_t*>(particleSystem) + (0x4 * static_cast<uint16_t>(ret)));
}

void Particle::dispatchParticle(const std::string& name, const Vector& pos)
{
	Vector copyPos = pos;

	void* addr = getCallAddr(name);
	const static auto _call = reinterpret_cast<void*>(m_callAddr);
	void* created = createParticle(_call, addr, &copyPos);

	// update pos, you can see bytes in set control point
	__asm add esp, 8
	*reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(created) + 0x3B4) = copyPos;

	using fn = void(__thiscall*)(void*, int, Vector*);
	const static auto setControlParticlePoint = reinterpret_cast<fn>(utilities::patternScan(CLIENT_DLL, SET_PARTICLE_POINT));

	setControlParticlePoint(created, 0, &copyPos);
	setControlParticlePoint(created, 1, &copyPos);
}

// because calling ParticleStart or doing this by DispatchEvent did not work
// have to wrap this for new particle keyvalues method, new because judging by xrefs there was some "old" method
__declspec(naked) void* Particle::createParticle(void* caller, void* addr, Vector* pos)
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