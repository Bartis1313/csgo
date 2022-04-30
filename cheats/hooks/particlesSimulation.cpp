#include "hooks.hpp"

#include <array>
#include <string>

#include "../../SDK/CParticelCollection.hpp"
#include "../../config/vars.hpp"

constexpr std::array mollyNames =
{
	"explosion_molotov_air",
	"extinguish_fire",
	"molotov_groundfire",
	"molotov_groundfire_fallback",
	"molotov_groundfire_fallback2",
	"molotov_explosion",
	"weapon_molotov_held",
	"weapon_molotov_fp",
	"weapon_molotov_thrown",
};

constexpr std::array smokenames =
{
	"explosion_smokegrenade",
	"explosion_smokegrenade_fallback"
};

constexpr std::array bloodnames =
{
	"blood_impact_light",
	"blood_impact_medium",
	"blood_impact_heavy",
	"blood_impact_light_headshot"
};

// do searches by multithread?
void __fastcall hooks::particlesSimulations::hooked(CParticleCollection* thisPtr, void* edx)
{
	original(thisPtr);

	if (!config.get<bool>(vars.bEditEffects))
		return;

	CParticleCollection* root = thisPtr;
	while (root->m_parent)
		root = root->m_parent;

	std::string_view name = root->m_def.m_obj->m_name.m_buffer;
	Color colorMolly = config.get<Color>(vars.cEditMolotov);
	Color colorBlood = config.get<Color>(vars.cEditBlood);
	float smokeAlpha = config.get<float>(vars.fSmokeAlpha);

	if (auto itr = std::find(smokenames.cbegin(), smokenames.cend(), name); itr != smokenames.cend())
	{
		for (size_t i = 0; i < thisPtr->m_activeParticles; i++)
		{
			thisPtr->m_particleAttributes.modulateAlpha(smokeAlpha, i);
		}
	}

	if (auto itr = std::find(bloodnames.cbegin(), bloodnames.cend(), name); itr != bloodnames.cend())
	{
		for (size_t i = 0; i < thisPtr->m_activeParticles; i++)
		{
			thisPtr->m_particleAttributes.modulateColor(colorBlood, i);
		}
	}

	if (auto itr = std::find(mollyNames.cbegin(), mollyNames.cend(), name); itr != mollyNames.cend())
	{
		for (size_t i = 0; i < thisPtr->m_activeParticles; i++)
		{
			thisPtr->m_particleAttributes.modulateColor(colorMolly, i);
		}
	}
}