#include "game.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/CUserCmd.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>

bool game::isAvailable()
{
	if (!localPlayer)
		return false;

	if (!memory::interfaces::engine->isConnected())
		return false;

	if (!memory::interfaces::engine->isInGame())
		return false;

	return true;
}

float game::serverTime(CUserCmd* cmd)
{
	static int tick;
	static CUserCmd* lastCmd;

	if (cmd)
	{
		if (!lastCmd || lastCmd->m_predicted)
			tick = localPlayer->m_nTickBase();
		else
			tick++;
		lastCmd = cmd;
	}
	return tick * memory::interfaces::globalVars->m_intervalPerTick;
}

void LocalPlayer::init()
{
	m_local = memory::localPlayer();
}

uint32_t game::timeToTicks(float time)
{
	return static_cast<uint32_t>(0.5f + time / memory::interfaces::globalVars->m_intervalPerTick);
}

float game::ticksToTime(uint32_t ticks)
{
	return memory::interfaces::globalVars->m_intervalPerTick * static_cast<float>(ticks);
}

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/183347-bomb-damage-indicator.html
float game::scaleDamageArmor(float dmg, const float armor)
{
	if (armor > 0.0f)
	{
		float newDmg = dmg * 0.5f;
		float scaledArmor = (dmg - newDmg) * 0.5f;

		if (scaledArmor > armor)
		{
			scaledArmor = armor * (1.0f / 0.5f);
			newDmg = dmg - scaledArmor;
		}
		dmg = newDmg;
	}

	return dmg;
}

#include <SDK/CHudChat.hpp>

bool game::isChatOpen()
{
	const auto chat = findHudElement<CHudChat*>("CCSGO_HudChat");
	return chat->m_isOpen;
}

float game::getScaledFont(const Vec3& source, const Vec3& destination, const float division, const float min, const float max)
{
	const float dist = source.distTo(destination);
	const float fontSize = std::clamp(division / (dist / division), min, max);
	return fontSize;
}

#include <SDK/CStudioHdr.hpp>

WeaponIndex game::getNadeByClass(int idx, Studiohdr_t* studio)
{
	switch (std::string_view name = studio->m_name; idx)
	{
	case CBaseCSGrenadeProjectile:
	{
		if (name.find("ggrenade") != std::string::npos)
			return WEAPON_HEGRENADE;
		else
			return WEAPON_FLASHBANG;
	}
	case CSmokeGrenadeProjectile:
		return WEAPON_SMOKEGRENADE;
	case CMolotovProjectile:
	{
		if (name.find("molotov") != std::string::npos)
			return WEAPON_MOLOTOV;
		else
			return WEAPON_INCGRENADE;
	}
	case CDecoyProjectile:
		return WEAPON_DECOY;
	default:
		break;
	}

	return WEAPON_NONE;
}

Vec3 game::getViewAngles()
{
	Vec3 ret;
	memory::interfaces::engine->getViewAngles(ret);

	return ret;
}

#include <SDK/CGameMovement.hpp>
#include <SDK/IWeapon.hpp>

Vec3 game::getFixedPunch()
{
	static auto prevPunch = Vec3{};
	if (!game::localPlayer->getActiveWeapon())
		return game::localPlayer->m_aimPunchAngle();

	float time = game::localPlayer->getActiveWeapon()->m_flNextPrimaryAttack() - memory::interfaces::globalVars->m_curtime;
	int ticks = timeToTicks(time);

	const auto& punchNow = game::localPlayer->m_aimPunchAngle();

	if (ticks <= 0)
		prevPunch = punchNow;
	else
	{
		for (int i = 0; i < ticks; i++)
		{
			memory::interfaces::gameMovement->decayAimPunchAngle();
		}
	}

	auto& decayed = game::localPlayer->m_aimPunchAngle();
	game::localPlayer->m_aimPunchAngle() = punchNow;

	if (const auto info = game::localPlayer->getActiveWeapon()->getWpnInfo())
	{
		const float lerped = std::clamp(time / info->m_cycleTime, 0.0f, 1.0f);

		printf("lerped %f\n", lerped);

		decayed = prevPunch.lerp(decayed, lerped);
	}

	return decayed;
}

#include <d3d9.h>
#include <d3dx9.h>
#include <unordered_map>
#include <SDK/IBaseFileSystem.hpp>

#pragma warning (disable : 4244)
#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>
#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvgrast.h>

std::optional<game::BitMapData> getSvgIcon(char* data, const float scale)
{
	constexpr float csDPI = 96.0f;
	NSVGimage* image = nsvgParse(data, "px", csDPI);
	if (!image)
	{
		nsvgDelete(image);
		return std::nullopt;
	}

	NSVGrasterizer* rasterizer = nsvgCreateRasterizer();
	if (!rasterizer)
	{
		nsvgDeleteRasterizer(rasterizer);
		nsvgDelete(image);

		return std::nullopt;
	}

	const auto width = static_cast<size_t>(image->width * scale);
	const auto height = static_cast<size_t>(image->height * scale);
	const auto imgData = std::make_unique<uint8_t[]>(width * height * 4);
	nsvgRasterize(rasterizer, image, 0, 0, scale, imgData.get(), width, height, width * 4);

	LPDIRECT3DTEXTURE9 texture;
	if (memory::interfaces::dx9Device->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL) != D3D_OK)
		return std::nullopt;

	D3DLOCKED_RECT lockedRect;
	constexpr UINT levelLock = 0U;
	if (texture->LockRect(levelLock, &lockedRect, NULL, 0) != D3D_OK)
		return std::nullopt;

	for (size_t y = 0; y < height; ++y)
	{
		const auto src = imgData.get() + y * width * 4;
		const auto dst = static_cast<uint8_t*>(lockedRect.pBits) + y * lockedRect.Pitch;
		std::ranges::copy(src, src + width * 4, dst);
	}

	texture->UnlockRect(levelLock);

	const auto fwidth = image->width * scale;
	const auto fheight = image->width * scale;

	nsvgDeleteRasterizer(rasterizer);
	nsvgDelete(image);

	return game::BitMapData{ .width = fwidth, .height = fheight, .texture = texture };
}

std::optional<game::BitMapData> getIconTexture(const std::string& path, float scale)
{
	const auto file = memory::interfaces::baseFileSystem->open(path.c_str(), "r", "GAME");
	if (!file)
		return std::nullopt;

	std::vector<char> buffer(memory::interfaces::baseFileSystem->size(file));

	memory::interfaces::baseFileSystem->read(buffer.data(), buffer.size(), file);
	memory::interfaces::baseFileSystem->close(file);

	const auto texture = getSvgIcon(buffer.data(), scale);
	if (!texture.has_value())
		return std::nullopt;

	return texture;
}

game::BitMapData game::getWeaponIcon(const std::string_view weapon)
{
	static std::unordered_map<std::string_view, BitMapData> icons;

	if (const auto itr = icons.find(weapon); itr != icons.end())
		return itr->second;

	// remove weapon_ prefix!
	const std::string path = std::format("materials/panorama/images/icons/equipment/{}.svg", weapon.substr(7));
	const auto maybetexture = getIconTexture(path, 1.0f);

	assert(maybetexture.has_value() == true);
	icons.emplace(weapon, maybetexture.value());

	return maybetexture.value();
}

game::BitMapData game::getImage(const std::string& path)
{
	static std::unordered_map<std::string, BitMapData> pics;

	if (const auto itr = pics.find(path); itr != pics.end())
		return itr->second;

	const auto file = memory::interfaces::baseFileSystem->open(path.c_str(), "r", "GAME");
	assert(file != nullptr);

	std::vector<char> buffer(memory::interfaces::baseFileSystem->size(file));

	memory::interfaces::baseFileSystem->read(buffer.data(), buffer.size(), file);
	memory::interfaces::baseFileSystem->close(file);

	LPDIRECT3DTEXTURE9 texture;
	const auto hrRes = D3DXCreateTextureFromFileInMemory(memory::interfaces::dx9Device(), buffer.data(), buffer.size(), &texture);
	assert(hrRes == D3D_OK);

	D3DSURFACE_DESC desc;
	texture->GetLevelDesc(0, &desc);

	const auto bitmap = game::BitMapData{ .width = static_cast<float>(desc.Width), .height = static_cast<float>(desc.Height), .texture = texture };
	pics.emplace(path, bitmap);

	return bitmap;
}
