#include "projectiles.hpp"

#include "../../../../../SDK/ILocalize.hpp"
#include "../../../../../SDK/IWeapon.hpp"
#include "../../../../../SDK/IVModelInfo.hpp"
#include "../../../../../SDK/interfaces/interfaces.hpp"

#include "../../../../game.hpp"
#include "../../../../globals.hpp"
#include "../../../../../config/vars.hpp"
#include "../../../../../utilities/utilities.hpp"
#include "../../../../../utilities/renderer/renderer.hpp"

#include "nadewarn.hpp"

void Projectiles::run(Entity_t* ent, int classID)
{
	if(!game::isAvailable())
		return;

	auto model = ent->getModel();
	if (!model)
		return;

	auto studio = interfaces::modelInfo->getStudioModel(model);
	if (!studio)
		return;

	auto wpn = reinterpret_cast<Weapon_t*>(ent);
	if (!wpn) // should not ever happen
		return;

	constexpr auto goodID = [=](const int idx) // needed because some nades are special
	{
		constexpr std::array arrid =
		{
			CBaseCSGrenadeProjectile,
			CDecoyProjectile,
			CMolotovProjectile,
		};
		auto itr = std::find(arrid.cbegin(), arrid.cend(), idx);
		return itr != arrid.cend();
	};

	if (std::string_view projectileName = studio->m_name; (projectileName.find(XOR("thrown")) != std::string::npos || goodID(classID)))
	{
		if (config.get<bool>(vars.bDrawProjectiles))
		{
			std::pair<std::string, CfgColor> nades;

			switch (g_GrenadeWarning.getIndexByClass(classID, studio))
			{
			case WEAPON_FLASHBANG:
			{
				nades = { XOR("FLASHBANG"), config.get<CfgColor>(vars.cFlashBang) };
				break;
			}
			case WEAPON_HEGRENADE:
			{
				if (wpn->m_nExplodeEffectTickBegin() < 1) // prevent too long time
					nades = { XOR("GRENADE"), config.get<CfgColor>(vars.cGranede) };
				break;
			}
			case WEAPON_MOLOTOV:
			{
				nades = { XOR("MOLOTOV"), config.get<CfgColor>(vars.cMolotov) };
				break;
			}
			case WEAPON_INCGRENADE:
			{
				nades = { XOR("FIRE INC"), config.get<CfgColor>(vars.cIncediary) };
				break;
			}
			case WEAPON_SMOKEGRENADE:
			{
				if (!reinterpret_cast<Smoke_t*>(wpn)->m_nSmokeEffectTickBegin()) // prevent too long time
					nades = { XOR("SMOKE"), config.get<CfgColor>(vars.cSmoke) };
				break;
			}
			case WEAPON_DECOY:
			{
				nades = { XOR("DECOY"), config.get<CfgColor>(vars.cDecoy) }; // this nade time is also too long, check velocity if it's very low
				break;
			}
			case WEAPON_NONE: // understand as NADE_NONE
				return;
			}

			if (Box box; utilities::getBox(ent, box))
				imRender.text(box.x + box.w / 2, box.y + box.h + 2, ImFonts::verdana12, nades.first, true, nades.second.getColor());
		}
	}
	else if (projectileName.find(XOR("dropped")) != std::string::npos && classID != CPlantedC4) // add more if needed
	{
		if (Box box; utilities::getBox(ent, box) && config.get<bool>(vars.bDrawDropped))
		{
			float fontSize = utilities::getScaledFont(ent->absOrigin(), game::localPlayer()->absOrigin(), 60.0f, 11.0f, 16.0f);
			using cont = std::vector<bool>; // container
			float padding = 0.0f;

			auto getTextSize = [=](const std::string& text, ImFont* font = ImFonts::verdana12)
			{
				return font->CalcTextSizeA(fontSize, std::numeric_limits<float>::max(), 0.0f, text.c_str());
			};

			if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::BOX))) // startpoint - no pad
			{
				CfgColor cfgCol = config.get<CfgColor>(vars.cDrawDropped);

				imRender.drawRect(box.x - 1.0f, box.y - 1.0f, box.w + 2.0f, box.h + 2.0f, Colors::Black);
				imRender.drawRect(box.x + 1.0f, box.y + 1.0f, box.w - 2.0f, box.h - 2.0f, Colors::Black);
				imRender.drawRect(box.x, box.y, box.w, box.h, cfgCol.getColor());
			}
			if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::AMMO)) && !wpn->isNonAimable()) // no pad font logic, we just draw extra box
			{
				constexpr float startPad = 3.0f;

				imRender.drawRectFilled(box.x - 1.0f, box.y + box.h - 1.0f + startPad, box.w + 2.0f, 4.0f, Colors::Black);
				imRender.drawRectFilled(box.x, box.y + box.h + startPad,
					wpn->m_iClip1() * box.w / wpn->getWpnInfo()->m_maxClip1, 2.0f, config.get<CfgColor>(vars.cDrawDropped).getColor());

				padding += 4.0f;
			}
			if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::TEXT)))
			{
				auto name = config.get<bool>(vars.bDrawWeaponTranslate)
					? interfaces::localize->findAsUTF8(wpn->getWpnInfo()->m_WeaponName)
					: wpn->getWpnName();
				imRender.text(box.x + box.w / 2, box.y + box.h + 2 + padding, fontSize, ImFonts::verdana12,
					name, true, config.get<CfgColor>(vars.cDropped).getColor());

				auto textSize = getTextSize(name);
				padding += textSize.y;
			}
			if (config.get<cont>(vars.vDroppedFlags).at(E2T(DroppedFlags::ICON)))
			{
				auto name = utilities::u8toStr(wpn->getIcon());
				imRender.text(box.x + box.w / 2, box.y + box.h + 2 + padding, fontSize, ImFonts::icon,
					name, true, config.get<CfgColor>(vars.cDropped).getColor());

				auto textSize = getTextSize(name, ImFonts::icon);
				padding += textSize.y;
			}
		}
	}
}