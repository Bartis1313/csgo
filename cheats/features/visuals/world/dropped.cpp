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
#include <utilities/utilities.hpp>

void DroppedWeapons::draw()
{
	if (!vars::visuals->esp->dropped->enabled)
		return;

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::WEAPON))
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

		auto flags = vars::visuals->esp->dropped->flags;

		if (flags.at(E2T(DroppedFlags::BOX))) // startpoint - no pad
		{
			imRender.drawRect(box.x - 1.0f, box.y - 1.0f, box.w + 2.0f, box.h + 2.0f, Colors::Black.getColorEditAlpha(alpha));
			imRender.drawRect(box.x + 1.0f, box.y + 1.0f, box.w - 2.0f, box.h - 2.0f, Colors::Black.getColorEditAlpha(alpha));
			imRender.drawRect(box.x, box.y, box.w, box.h, vars::visuals->esp->dropped->color().getColorEditAlpha(alpha));
		}
		if (flags.at(E2T(DroppedFlags::AMMO)) && !wpn->isNonAimable()) // no pad font logic, we just draw extra box
		{
			constexpr float startPad = 3.0f;

			imRender.drawRectFilled(box.x - 1.0f, box.y + box.h - 1.0f + startPad, box.w + 2.0f, 4.0f, Colors::Black.getColorEditAlpha(alpha));
			imRender.drawRectFilled(box.x, box.y + box.h + startPad,
				wpn->m_iClip1() * box.w / wpn->getWpnInfo()->m_maxClip1, 2.0f, vars::visuals->esp->dropped->color().getColorEditAlpha(alpha));

			padding += 4.0f;
		}
		if (flags.at(E2T(DroppedFlags::TEXT)))
		{
			auto name = vars::visuals->esp->weaponBar->translate
				? interfaces::localize->findAsUTF8(wpn->getWpnInfo()->m_WeaponName)
				: wpn->getWpnName();
			imRender.text(box.x + box.w / 2, box.y + box.h + 2 + padding, fontSize, ImFonts::verdana12,
				name, true, vars::visuals->esp->dropped->color().getColorEditAlpha(alpha));

			auto textSize = getTextSize(name);
			padding += textSize.y;
		}
		if (flags.at(E2T(DroppedFlags::ICON)))
		{
			auto name = utilities::u8toStr(wpn->getIcon());
			imRender.text(box.x + box.w / 2, box.y + box.h + 2 + padding, fontSize, ImFonts::icon,
				name, true, vars::visuals->esp->dropped->color().getColorEditAlpha(alpha));

			auto textSize = getTextSize(name, ImFonts::icon);
			padding += textSize.y;
		}
	}
}
