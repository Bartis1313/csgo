#pragma once

#include <SDK/math/Vector.hpp>

#include <string>

#define PARTICLE_MANUAL true

#if PARTICLE_MANUAL == true
class Particle
{	
public:
	static void dispatchParticle(const std::string& name, const Vec3& pos);
private:
	static void* getCallAddr(const std::string& name);
	// caller is passed because it's a naked function
	static void* createParticle(void* caller, void* addr, Vec3* pos);
};
#else
// recommended solution
// we will completely null special such as ent control to particle
namespace particle
{
	void dispatchParticleEffect(const std::string_view name, const Vec3& pos, const Vec3& angles);
	void stopParticleEffect(const std::string_view name);
}
#endif