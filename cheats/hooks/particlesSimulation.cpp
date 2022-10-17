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

void __fastcall hooks::particlesSimulations::hooked(FAST_ARGS)
{
	original(thisptr);

	auto ptr = reinterpret_cast<CParticleCollection*>(thisptr);
	CParticleCollection* root = ptr;
	while (root->m_parent)
		root = root->m_parent;

	std::string_view name = root->m_def.m_obj->m_name.m_buffer;
	Color colorMolly = vars::visuals->world->particles->colorMolotov();
	Color colorBlood = vars::visuals->world->particles->colorBlood();
	Color colorSmoke = vars::visuals->world->particles->colorSmoke();

	if (vars::visuals->world->particles->enabledSmoke)
	{
		if (auto itr = std::find(smokenames.cbegin(), smokenames.cend(), name); itr != smokenames.cend())
		{
			for (auto i : std::views::iota(0, ptr->m_activeParticles))
			{
				ptr->m_particleAttributes.modulateColor(colorSmoke, i);
			}
		}
	}

	if (vars::visuals->world->particles->enabledBlood)
	{
		if (auto itr = std::find(bloodnames.cbegin(), bloodnames.cend(), name); itr != bloodnames.cend())
		{
			for (auto i : std::views::iota(0, ptr->m_activeParticles))
			{
				ptr->m_particleAttributes.modulateColor(colorBlood, i);
			}
		}
	}

	if (vars::visuals->world->particles->enabledMolotov)
	{
		if (auto itr = std::find(mollyNames.cbegin(), mollyNames.cend(), name); itr != mollyNames.cend())
		{
			for (auto i : std::views::iota(0, ptr->m_activeParticles))
			{
				ptr->m_particleAttributes.modulateColor(colorMolly, i);
			}
		}
	}
}