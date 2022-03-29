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
	const auto time = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&time), XOR("%d:%m:%Y-%X"));
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

	LF(MessageBoxA)(nullptr, std::format(XOR("Pattern scanning failed! mod: {} mask: {}"), mod, mask).c_str(), XOR("Bartis hack"), MB_OK | MB_ICONWARNING);

	return 0;
}

#include "../SDK/structs/Entity.hpp"
#include "../SDK/ICollideable.hpp"
#include "renderer/renderer.hpp"
#include "math/math.hpp"

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

	// will never happen
	/*if (!points.data())
		return false;*/

	const auto& tranFrame = ent->m_rgflCoordinateFrame();

	float left = std::numeric_limits<float>::max();
	float top = std::numeric_limits<float>::max();
	float right = -std::numeric_limits<float>::max();
	float bottom = -std::numeric_limits<float>::max();

	std::array<Vector2D, 8> screen = {};

	for (int i = 0; i < 8; i++)
	{
		if (!imRender.worldToScreen(math::transformVector(points.at(i), tranFrame), screen.at(i)))
			return false;

		left = std::min(left, screen.at(i).x);
		top = std::min(top, screen.at(i).y);
		right = std::max(right, screen.at(i).x);
		bottom = std::max(bottom, screen.at(i).y);
	}

	box.x = left;
	box.y = top;
	box.w = right - left;
	box.h = bottom - top;

	return true;
}

bool utilities::getBox3D(Entity_t* ent, Box3D& box)
{
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

	// will never happen
	/*if (!points.data())
		return false;*/

	const auto& tranFrame = ent->m_rgflCoordinateFrame();

	std::array<Vector2D, 8> screen = {};

	for (int i = 0; i < 8; i++)
	{
		if (!imRender.worldToScreen(math::transformVector(points.at(i), tranFrame), screen.at(i)))
			return false;

		box.points.at(i) = screen.at(i); // get all 3D points
	}

	// get important points, eg: if you use 3d box, you want to render health by quads, not rects

	box.topleft = screen.at(7);
	box.topright = screen.at(6);
	box.bottomleft = screen.at(3);
	box.bottomright = screen.at(2);

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