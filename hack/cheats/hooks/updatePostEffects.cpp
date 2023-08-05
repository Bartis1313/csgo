#include "updatePostEffects.hpp"

#include <SDK/IMaterialSystem.hpp>
#include <config/vars.hpp>
#include <cheats/features/visuals/chams/factory/factory.hpp>
#include <cheats/game/game.hpp>

hooks::UpdatePostEffects::value hooks::UpdatePostEffects::hook(HACK_FAST_ARGS)
{
	IMaterial* blurOverlayMaterial{ };

	if (game::isAvailable())
	{
		static std::once_flag onceFlag;
		std::call_once(onceFlag, [&blurOverlayMaterial]() { blurOverlayMaterial = material::factory::findMaterial("dev/scope_bluroverlay", TEXTURE_GROUP_OTHER); });
	}

	if(blurOverlayMaterial)
		blurOverlayMaterial->setMaterialVarFlag(MATERIAL_VAR_NO_DRAW, vars::misc->scope->enabled);

	if (globals::isShutdown)
	{
		if(blurOverlayMaterial)
			blurOverlayMaterial->setMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
	}

	original(thisptr);

	if(vars::misc->scope->enabled)
		*memory::vignetteBlurStrengthPost() = 0.0f;
}