#pragma once

#include "../../classes/onlyInit.hpp"

#include <string>

class Entity_t;
struct Vector;

class Particle : public OnlyInitType
{
public:
	Particle()
	{
		m_hacks.push_back(this);
	}

	virtual void init();
	void dispatchParticle(const std::string& name, const Vector& pos);
private:
	void* getCallAddr(const std::string& name);
	// caller is passed because it's a naked function
	void* createParticle(void* caller, void* addr, Vector* pos);

	uintptr_t m_isCachedAddr;
	uintptr_t m_particleSystenAddr;
	uintptr_t m_findStringIndexAddr;
	uintptr_t m_callAddr;
};

[[maybe_unused]] inline auto g_Particle = Particle{};