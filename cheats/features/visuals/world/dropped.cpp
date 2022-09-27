#include "dropped.hpp"

#include "../../cache/cache.hpp"

#include <SDK/ILocalize.hpp>
#include <SDK/IWeapon.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/renderer/renderer.hpp>

void DroppedWeapons::init()
{

}

void DroppedWeapons::draw()
{
	if (!config.get<bool>(vars.bDrawDropped))
		return;

	for (auto [entity, idx, classID] : g_EntCache.getCache(EntCacheType::WEAPON))
	{
		auto handle = interfaces::entList->getClientFromHandle(entity->m_hOwnerEntity());
		if (handle)
			continue;

		auto wpn = reinterpret_cast<Weapon_t*>(entity);
		if (!wpn)
			continue;

		Box box{ entity };
		if (!box.isValid())
			continue;
	
		float fontSize = game::getScaledFont(entity->absOrigin(), game::localPlayer()->absOrigin(), 60.0f, 11.0f, 16.0f);
		using cont = std::vector<bool>;
		float padding = 0.0f;

		constexpr float maxDist = 25.0f;
		float alpha = (maxDist - entity->absOrigin().distToMeters(game::localPlayer->absOrigin())) / (maxDist / 1.5f);

		auto getTextSize = [=](const std::string& text, ImFont* font = ImFonts::verdana12)
		{
			return font->CalcTextSizeA(fontSize, std::numeric_limits<float>::max(), 0.0f, text.c_str());
		};

		if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::BOX))) // startpoint - no pad
		{
			CfgColor cfgCol = config.get<CfgColor>(vars.cDrawDropped);

			imRender.drawRect(box.x - 1.0f, box.y - 1.0f, box.w + 2.0f, box.h + 2.0f, Colors::Black.getColorEditAlpha(alpha));
			imRender.drawRect(box.x + 1.0f, box.y + 1.0f, box.w - 2.0f, box.h - 2.0f, Colors::Black.getColorEditAlpha(alpha));
			imRender.drawRect(box.x, box.y, box.w, box.h, cfgCol.getColor().getColorEditAlpha(alpha));
		}
		if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::AMMO)) && !wpn->isNonAimable()) // no pad font logic, we just draw extra box
		{
			constexpr float startPad = 3.0f;

			imRender.drawRectFilled(box.x - 1.0f, box.y + box.h - 1.0f + startPad, box.w + 2.0f, 4.0f, Colors::Black.getColorEditAlpha(alpha));
			imRender.drawRectFilled(box.x, box.y + box.h + startPad,
				wpn->m_iClip1() * box.w / wpn->getWpnInfo()->m_maxClip1, 2.0f, config.get<CfgColor>(vars.cDrawDropped).getColor().getColorEditAlpha(alpha));

			padding += 4.0f;
		}
		if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::TEXT)))
		{
			auto name = config.get<bool>(vars.bDrawWeaponTranslate)
				? interfaces::localize->findAsUTF8(wpn->getWpnInfo()->m_WeaponName)
				: wpn->getWpnName();
			imRender.text(box.x + box.w / 2, box.y + box.h + 2 + padding, fontSize, ImFonts::verdana12,
				name, true, config.get<CfgColor>(vars.cDropped).getColor().getColorEditAlpha(alpha));

			auto textSize = getTextSize(name);
			padding += textSize.y;
		}
		if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::ICON)))
		{
			auto name = utilities::u8toStr(wpn->getIcon());
			imRender.text(box.x + box.w / 2, box.y + box.h + 2 + padding, fontSize, ImFonts::icon,
				name, true, config.get<CfgColor>(vars.cDropped).getColor().getColorEditAlpha(alpha));

			auto textSize = getTextSize(name, ImFonts::icon);
			padding += textSize.y;
		}
	}
}
