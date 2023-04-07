#include "dropped.hpp"

#include "../../cache/cache.hpp"

#include <SDK/ILocalize.hpp>
#include <SDK/IWeapon.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <render/render.hpp>
#include <utilities/utilities.hpp>

void DroppedWeapons::draw()
{
	if (!vars::visuals->esp->dropped->enabled)
		return;
	
	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::WEAPON))
	{
		auto wpn = reinterpret_cast<Weapon_t*>(entity);
		if (!wpn)
			continue;

		auto handle = memory::interfaces::entList->getClientFromHandle(wpn->m_hOwnerEntity());
		if (handle)
			continue;

		Box box{ entity };
		if (!box.isValid())
			continue;
	
		const float fontSize = game::getScaledFont(entity->absOrigin(), game::localPlayer()->absOrigin(), 60.0f, 11.0f, 16.0f);
		using cont = std::vector<bool>;
		float padding = 0.0f;

		constexpr float maxDist = 25.0f;
		const float alpha = (maxDist - entity->absOrigin().distToMeters(game::localPlayer->absOrigin())) / (maxDist / 1.5f);

		if (alpha < 0.0f)
			continue;

		auto getTextSize = [=](const std::string& text, ImFont* font = ImFonts::verdana12)
		{
			return font->CalcTextSizeA(fontSize, std::numeric_limits<float>::max(), 0.0f, text.c_str());
		};

		const auto flags = vars::visuals->esp->dropped->flags;

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
			const auto name = vars::visuals->esp->weaponBar->translate
				? memory::interfaces::localize->findAsUTF8(wpn->getWpnInfo()->m_WeaponName)
				: wpn->getWpnName();
			imRender.text(box.x + box.w / 2, box.y + box.h + 2 + padding, fontSize, ImFonts::verdana12,
				name, true, vars::visuals->esp->dropped->color().getColorEditAlpha(alpha));

			auto textSize = getTextSize(name);
			padding += textSize.y;
		}
		if (flags.at(E2T(DroppedFlags::ICON)))
		{
			const auto iconData = game::getWeaponIcon(wpn->getWpnInfo()->m_consoleName);
			
			// I couldnt find in panorama scaling arguments passed by, it's usually -1.0f for them?
			auto goodSize = ImVec2{ iconData.width, iconData.height };
			goodSize = wpn->isGrenade() ? ImVec2{ 8.0f, 12.0f } : ImVec2{ 22.0f, 14.0f };

			imRender.drawImage(iconData.texture, ImVec2{ box.x + box.w / 2, box.y + box.h + 2 + padding },
				goodSize, vars::visuals->esp->dropped->color().getColorEditAlpha(alpha));

			padding += goodSize.y;
		}
	}
}
