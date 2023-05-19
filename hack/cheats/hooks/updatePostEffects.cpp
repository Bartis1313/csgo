#include "updatePostEffects.hpp"

#include <SDK/IMaterialSystem.hpp>
#include <config/vars.hpp>
#include <cheats/features/visuals/chams/factory/factory.hpp>

hooks::UpdatePostEffects::value hooks::UpdatePostEffects::hook(FAST_ARGS)
{
	static IMaterial* blurOverlayMaterial = material::factory::findMaterial("dev/scope_bluroverlay", TEXTURE_GROUP_OTHER);
	blurOverlayMaterial->setMaterialVarFlag(MATERIAL_VAR_NO_DRAW, vars::misc->scope->enabled);

	if (globals::isShutdown)
		blurOverlayMaterial->setMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);

	original(thisptr);

	if(vars::misc->scope->enabled)
		*memory::vignetteBlurStrengthPost() = 0.0f;
}