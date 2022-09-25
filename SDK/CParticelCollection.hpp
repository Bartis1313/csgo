#pragma once

#include "CUtlReference.hpp"
#include "CUtlString.hpp"
#include "Color.hpp"

#include <utilities/pad.hpp>

#define PARTICLE_ATTRIBUTE_TINT_RGB 6
#define PARTICLE_ATTRIBUTE_ALPHA	7

#define MAX_PARTICLE_ATTRIBUTES 24

struct CParticleAttributeAddressTable
{
	float* m_pAttributes[MAX_PARTICLE_ATTRIBUTES];
	size_t m_nFloatStrides[MAX_PARTICLE_ATTRIBUTES];

	float* floatAttribute(int attribute, int particleNum) const
	{
		int blockofs = particleNum / 4;
		return m_pAttributes[attribute] +
			m_nFloatStrides[attribute] * blockofs +
			(particleNum & 3);
	}
	// custom
	void modulateColor(const Color& color, int num)
	{
		/*
		0 : 0.948685
		1 : 0.942262
		2 : 0.898302
		3 : 0.932009
		4 : 0.885635
		5 : 0.878489
		6 : 0.717303
		7 : 0.840896
		8 : 0.670859
		*/

		auto rgb = floatAttribute(PARTICLE_ATTRIBUTE_TINT_RGB, num);
		auto a = floatAttribute(PARTICLE_ATTRIBUTE_ALPHA, num);


		rgb[0] = color.r();
		rgb[4] = color.g();
		rgb[8] = color.b();

		*a = color.a();
	}
	void modulateAlpha(float alpha, int num)
	{
		auto a = floatAttribute(PARTICLE_ATTRIBUTE_ALPHA, num);
		*a = alpha;
	}
};

class CParticleSystemDefinition
{
	PAD(308);
	CUtlString m_name;
};

class CParticleCollection
{
	PAD(48);
	int m_activeParticles;
	PAD(12);
	CUtlReference<CParticleSystemDefinition> m_def;
	PAD(60);
	CParticleCollection* m_parent;
	PAD(84);
	CParticleAttributeAddressTable m_particleAttributes;
};