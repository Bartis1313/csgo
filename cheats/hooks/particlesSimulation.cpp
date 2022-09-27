#include "hooks.hpp"

#include <SDK/CParticelCollection.hpp>
#include <config/vars.hpp>

#include <array>
#include <string>
#include <ranges>

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

void __fastcall hooks::particlesSimulations::hooked(CParticleCollection* thisPtr, void* edx)
{
	original(thisPtr);

	CParticleCollection* root = thisPtr;
	while (root->m_parent)
		root = root->m_parent;

	std::string_view name = root->m_def.m_obj->m_name.m_buffer;
	CfgColor colorMolly = config.get<CfgColor>(vars.cEditMolotov);
	CfgColor colorBlood = config.get<CfgColor>(vars.cEditBlood);
	CfgColor colorSmoke = config.get<CfgColor>(vars.cEditSmoke);

	if (config.get<bool>(vars.bEditEffectsSmoke))
	{
		if (auto itr = std::find(smokenames.cbegin(), smokenames.cend(), name); itr != smokenames.cend())
		{
			for (auto i : std::views::iota(0, thisPtr->m_activeParticles))
			{
				thisPtr->m_particleAttributes.modulateColor(colorSmoke.getColor(), i);
			}
		}
	}

	if (config.get<bool>(vars.bEditEffectsBlood))
	{
		if (auto itr = std::find(bloodnames.cbegin(), bloodnames.cend(), name); itr != bloodnames.cend())
		{
			for (auto i : std::views::iota(0, thisPtr->m_activeParticles))
			{
				thisPtr->m_particleAttributes.modulateColor(colorBlood.getColor(), i);
			}
		}
	}

	if (config.get<bool>(vars.bEditEffectsMoly))
	{
		if (auto itr = std::find(mollyNames.cbegin(), mollyNames.cend(), name); itr != mollyNames.cend())
		{
			for (auto i : std::views::iota(0, thisPtr->m_activeParticles))
			{
				thisPtr->m_particleAttributes.modulateColor(colorMolly.getColor(), i);
			}
		}
	}
}