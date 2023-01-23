#pragma once

#include <SDK/math/Vector.hpp>

#include <string>

class Particle
{	
public:
	static void dispatchParticle(const std::string& name, const Vec3& pos);
private:
	static void* getCallAddr(const std::string& name);
	// caller is passed because it's a naked function
	static void* createParticle(void* caller, void* addr, Vec3* pos);
};