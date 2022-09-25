#include "particle.hpp"

#include <SDK/interfaces/interfaces.hpp>
#include <SDK/math/Vector.hpp>
#include <utilities/utilities.hpp>
#include <gamememory/memory.hpp>

void Particle::init()
{

}

void* Particle::getCallAddr(const std::string& name)
{
	int ret = -1;
	if (g_Memory.m_particleIsCached()(g_Memory.m_particleSystem(), name.c_str()))
		g_Memory.m_particleFindStringIndex()(reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(g_Memory.m_particleSystem()) + 0x14), &ret, name.c_str()); // esi+14h

	if (ret == -1)
		return nullptr;
	// *v3 + 4 * v5 - > v5 is casted unsigned __int16
	return *reinterpret_cast<void**>(*reinterpret_cast<uintptr_t*>(g_Memory.m_particleSystem()) + (0x4 * static_cast<uint16_t>(ret)));
}

void Particle::dispatchParticle(const std::string& name, const Vector& pos)
{
	Vector copyPos = pos;

	void* addr = getCallAddr(name);
	const static auto _call = g_Memory.m_particleCall();
	void* created = createParticle(_call, addr, &copyPos);

	// update pos, you can see bytes in set control point
	__asm add esp, 8
	*reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(created) + 0x3B4) = copyPos;


	g_Memory.m_particleSetControlPoint()(created, 0, &copyPos);
	g_Memory.m_particleSetControlPoint()(created, 1, &copyPos);
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