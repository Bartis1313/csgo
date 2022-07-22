#include "utilities.hpp"

#include <Windows.h>
#include <Psapi.h>
#include <optional>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <cstddef>

#pragma warning(disable: 6001) // memory unallocated
#pragma warning(disable: 6054) // string terminated

std::string utilities::getTime()
{
	const auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;

	std::tm bt = {};
	localtime_s(&bt, &time);

	ss << std::put_time(&bt, XOR("%d:%m:%Y-%X")); 
	return ss.str();
}

uintptr_t utilities::patternScan(const std::string& mod, const std::string& mask, const uintptr_t offsetToAdd)
{
	MODULEINFO modInfo;
#ifndef _DEBUG
	LF(K32GetModuleInformation).cached()(LF(GetCurrentProcess).cached()(), LF(GetModuleHandleA).cached()(mod.c_str()), &modInfo, sizeof(MODULEINFO));
#else
	K32GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(mod.c_str()), &modInfo, sizeof(MODULEINFO));
#endif		
	uintptr_t ranageStart = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
	std::istringstream iss{ mask };
	std::vector<std::string> parts{ std::istream_iterator<std::string>{ iss }, std::istream_iterator<std::string>{} };
	std::vector<std::optional<std::byte>> actualPattern = {};

	std::for_each(parts.cbegin(), parts.cend(), [&](const std::string& str)
		{
			if (str == "?" || str == "??")
				actualPattern.emplace_back(std::nullopt);
			else
				actualPattern.emplace_back(static_cast<std::byte>(std::stoi(str, nullptr, 16)));
		});

	for (size_t i = 0; i < modInfo.SizeOfImage; i++)
	{
		if (auto check = [](std::byte* data, const std::vector<std::optional<std::byte>>& _mask)
			{
				for (const auto& _byte : _mask)
				{
					if (_byte && _byte.value() != *data)
						return false;
						data++;
				}
				return true;
			}; check(reinterpret_cast<std::byte*>(ranageStart + i), actualPattern))
		{
			return ranageStart + i + offsetToAdd;
		}
	}

	LF(MessageBoxA)(nullptr, FORMAT(XOR("Pattern scanning failed! mod: {} mask: {}"), mod, mask).c_str(), XOR("Bartis hack"), MB_OK | MB_ICONWARNING);

	return 0;
}

#include "../SDK/structs/Entity.hpp"
#include "../SDK/ICollideable.hpp"
#include "renderer/renderer.hpp"
#include "math/math.hpp"

bool utilities::getBox(Entity_t* ent, Box& box, Box3D& box3D)
{
	// pretty much nothing to explain here, using engine and detect mins/maxs
	const auto col = ent->collideable();
	if (!col)
		return false;

	const auto& min = col->OBBMins();
	const auto& max = col->OBBMaxs();

	const auto& matrixWorld = ent->renderableToWorldTransform();

	float left = std::numeric_limits<float>::max();
	float top = std::numeric_limits<float>::max();
	float right = -std::numeric_limits<float>::max();
	float bottom = -std::numeric_limits<float>::max();

	std::array points =
	{
		Vector{ min.x, min.y, min.z },
		Vector{ min.x, max.y, min.z },
		Vector{ max.x, max.y, min.z },
		Vector{ max.x, min.y, min.z },
		Vector{ min.x, min.y, max.z },
		Vector{ min.x, max.y, max.z },
		Vector{ max.x, max.y, max.z },
		Vector{ max.x, min.y, max.z }
	};

#ifdef SURFACE_DRAWING
	std::array<Vector2D, 8> screen = {};
#else
	std::array<ImVec2, 8> screen = {};
#endif

	for (size_t i = 0; auto& el : screen)
	{
		if (!imRender.worldToScreen(math::transformVector(points.at(i), matrixWorld), el))
			return false;

		left = std::min(left, el.x);
		top = std::min(top, el.y);
		right = std::max(right, el.x);
		bottom = std::max(bottom, el.y);

		box3D.points.at(i) = el;

		i++;
	}

	box.x = left;
	box.y = top;
	box.w = right - left;
	box.h = bottom - top;

	// get important points, eg: if you use 3d box, you want to render health by quads, not rects

	box3D.topleft = screen.at(7);
	box3D.topright = screen.at(6);
	box3D.bottomleft = screen.at(3);
	box3D.bottomright = screen.at(2);

	return true;
}

bool utilities::getBox(Entity_t* ent, Box& box)
{
	// pretty much nothing to explain here, using engine and detect mins/maxs
	const auto col = ent->collideable();
	if (!col)
		return false;

	const auto& min = col->OBBMins();
	const auto& max = col->OBBMaxs();

	std::array points =
	{
		Vector{ min.x, min.y, min.z },
		Vector{ min.x, max.y, min.z },
		Vector{ max.x, max.y, min.z },
		Vector{ max.x, min.y, min.z },
		Vector{ min.x, min.y, max.z },
		Vector{ min.x, max.y, max.z },
		Vector{ max.x, max.y, max.z },
		Vector{ max.x, min.y, max.z }
	};

	const auto& matrixWorld = ent->renderableToWorldTransform();

	float left = std::numeric_limits<float>::max();
	float top = std::numeric_limits<float>::max();
	float right = -std::numeric_limits<float>::max();
	float bottom = -std::numeric_limits<float>::max();

#ifdef SURFACE_DRAWING
	std::array<Vector2D, 8> screen = {};
#else
	std::array<ImVec2, 8> screen = {};
#endif

	for (size_t i = 0; auto & el : screen)
	{
		if (!imRender.worldToScreen(math::transformVector(points.at(i), matrixWorld), el))
			return false;

		left = std::min(left, el.x);
		top = std::min(top, el.y);
		right = std::max(right, el.x);
		bottom = std::max(bottom, el.y);

		i++;
	}

	box.x = left;
	box.y = top;
	box.w = right - left;
	box.h = bottom - top;

	return true;
}

size_t utilities::inByteOrder(const size_t netLong)
{
	std::array<std::byte, 4> data = {};
	memcpy(&data, &netLong, data.size());

	return (static_cast<size_t>(data.at(3)) << 0)
		| (static_cast<size_t>(data.at(2)) << 8)
		| (static_cast<size_t>(data.at(1)) << 16)
		| (static_cast<size_t>(data.at(0)) << 24);
}

std::string utilities::getKeyName(const uint32_t virtualKey)
{
#ifdef _DEBUG
	uint32_t scanCode = MapVirtualKeyA(virtualKey, MAPVK_VK_TO_VSC);
#else
	uint32_t scanCode = LF(MapVirtualKeyA).cached()(virtualKey, MAPVK_VK_TO_VSC);
#endif

	// because MapVirtualKey strips the extended bit for some keys
	switch (virtualKey)
	{
	case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN: // arrow keys
	case VK_PRIOR: case VK_NEXT: // page up and page down
	case VK_END: case VK_HOME:
	case VK_INSERT: case VK_DELETE:
	case VK_DIVIDE: // numpad slash
	case VK_NUMLOCK:
	{
		scanCode |= 0x100; // set extended bit
		break;
	}
	case 1:
		return XOR("LMB");
		break;
	case 2:
		return XOR("RMB");
		break;
	case 4:
		return XOR("MMB");
		break;
	case 5:
		return XOR("Side 1");
		break;
	case 6:
		return XOR("Side 2");
		break;
	}

#ifdef _DEBUG
	if (char keyName[50]; GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName)) != 0)
	{
		return keyName;
	}
	else
	{
		return XOR("[None]");
	}
#else
	if (char keyName[50]; LF(GetKeyNameTextA).cached()(scanCode << 16, keyName, sizeof(keyName)) != 0)
	{
		return keyName;
	}
	else
	{
		return XOR("[None]");
	}
#endif
}

std::string utilities::toLowerCase(const std::string& str)
{
	std::string result = str;
	std::for_each(result.begin(), result.end(), [](char& el)
		{
			el = ::tolower(el);
		});
	return result;
}

std::string utilities::toUpperCase(const std::string& str)
{
	std::string result = str;
	std::for_each(result.begin(), result.end(), [](char& el)
		{
			el = ::toupper(el);
		});
	return result;
}

std::vector<std::string> utilities::splitStr(const std::string& str, char limit)
{
	std::vector<std::string> res;
	std::stringstream content{ str };
	std::string word;
	while (std::getline(content, word, limit))
	{
		res.push_back(word);
	}
	return res;
}

uint32_t utilities::getKey(const uint32_t vKey)
{
#ifdef _DEBUG
	return GetAsyncKeyState(vKey);
#else
	return LF(GetAsyncKeyState).cached()(vKey);
#endif
}

float utilities::scaleDamageArmor(float dmg, const float armor)
{
	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/183347-bomb-damage-indicator.html
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

std::string utilities::u8toStr(const std::u8string& u8str)
{
	return std::string{ u8str.cbegin(), u8str.cend() };
}

uintptr_t* utilities::findHudElement(const std::string_view name)
{
	const static auto hudPtr = *reinterpret_cast<void**>(patternScan(CLIENT_DLL, CSGO_HUD, 0x1));
	using fn = uintptr_t*(__thiscall*)(void* /*uintptr_t*/, const char*);

	const auto static findh = reinterpret_cast<fn>(patternScan(CLIENT_DLL, FIND_ELEMENT));
	return findh(hudPtr, name.data());
}

#include "../SDK/CHudChat.hpp"

bool utilities::isChatOpen()
{
	const auto chat = reinterpret_cast<CHudChat*>(findHudElement(XOR("CCSGO_HudChat")));
	return chat->m_isOpen;
}
