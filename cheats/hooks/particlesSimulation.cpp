#include "hooks.hpp"

#include <array>
#include <string>

#include "../../SDK/CParticelCollection.hpp"
#include "../../config/vars.hpp"

constexpr std::array names =
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

void __fastcall hooks::particlesSimulations::hooked(CParticleCollection* thisPtr, void* edx)
{
	original(thisPtr);

	if (!config.get<bool>(vars.bEditMolotov))
		return;

	CParticleCollection* root = thisPtr;
	while (root->m_parent)
		root = root->m_parent;

	std::string_view name = root->m_def.m_obj->m_name.m_buffer;
	Color color = config.get<Color>(vars.cEditMolotov);

	if (auto itr = std::find(names.cbegin(), names.cend(), name); itr != names.cend())
	{
		for (size_t i = 0; i < thisPtr->m_activeParticles; i++)
		{
			thisPtr->m_particleAttributes.modulateColor(color, i);
		}
	}
}