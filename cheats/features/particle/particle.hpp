#pragma once

#include <classes/onlyInit.hpp>

#include <SDK/math/Vector.hpp>

#include <string>

class Entity_t;

class Particle : public OnlyInitType
{
public:
	constexpr Particle() :
		OnlyInitType{}
	{}

	virtual void init();
	void dispatchParticle(const std::string& name, const Vec3& pos);
private:
	void* getCallAddr(const std::string& name);
	// caller is passed because it's a naked function
	void* createParticle(void* caller, void* addr, Vec3* pos);
};

[[maybe_unused]] inline auto g_Particle = Particle{};