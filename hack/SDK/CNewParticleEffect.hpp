#pragma once


#include "math/Vector.hpp"
#include "math/matrix.hpp"
#include "Enums.hpp"

#include "CParticelCollection.hpp"

class Entity_t;

class CNewParticleEffect
{
public:
	CParticleCollection* getParticleCollection() { return reinterpret_cast<CParticleCollection*>(this + 0x10); } // inherit
	 
	void setControlPoint(int whichPoint, const Vec3& v);
	void setControlPointEntity(int whichPoint, Entity_t* ent);
	//void setDrawOnlyForSplitScreenUse(int slot);

	// thisptr passed by stack, some inlined wrapper probs
	static CNewParticleEffect* create(const char* name, ParticleAttachment_t attachType, int attachPoint = -1, Vec3 vecOriginOffset = Vec3{ }, Matrix3x4* vecOffsetMatrix = nullptr);
};