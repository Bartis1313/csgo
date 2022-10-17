#pragma once

#include <classes/onlyInit.hpp>

#include <SDK/math/Vector.hpp>

#include <string>

class Entity_t;

class Particle : protected OnlyInitType
{
public:
	constexpr Particle() :
		OnlyInitType{}
	{}
	
public:
	void dispatchParticle(const std::string& name, const Vec3& pos);
protected:
	virtual void init() override;
private:
	void* getCallAddr(const std::string& name);
	// caller is passed because it's a naked function
	void* createParticle(void* caller, void* addr, Vec3* pos);
};

GLOBAL_FEATURE(Particle);