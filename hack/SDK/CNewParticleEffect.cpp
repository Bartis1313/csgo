#include "CNewParticleEffect.hpp"

#include <gamememory/memory.hpp>

void CNewParticleEffect::setControlPoint(int whichPoint, const Vec3& v)
{
	memory::newParticleSetControlPoint()(this, whichPoint, std::cref(v));
}

void CNewParticleEffect::setControlPointEntity(int whichPoint, Entity_t* ent)
{
	memory::newParticleSetControlPointEnt()(this, whichPoint, ent);
}

CNewParticleEffect* CNewParticleEffect::create(const char* name, ParticleAttachment_t attachType, int attachPoint, Vec3 vecOriginOffset, Matrix3x4* vecOffsetMatrix)
{
	return memory::newParticleCreate()(name, attachType, attachPoint, vecOriginOffset, vecOffsetMatrix);
}