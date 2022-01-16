#include "utilities.hpp"
#include <Psapi.h>
#include <filesystem>
#include <ShlObj_core.h>
#include "renderer/renderer.hpp"
#include "../SDK/structs/Entity.hpp"
#include <optional>

#undef max
#undef min

namespace fs = std::filesystem;

std::string utilities::getFolder()
{
	CHAR documents[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents);

	return std::string(documents);
}

bool utilities::prepareDirectories()
{
	bool result = false;
	try
	{
		fs::current_path(__DOCUMENTS);
		result = fs::create_directories(XOR("Bartis_internal/csgo"));
	}
	catch (const fs::filesystem_error& err)
	{
		throw std::runtime_error(err.what());
	}

	LOG(LOG_INFO, result ? XOR("Created new folder, prepared directories") : XOR("Folder already exists with config, prepared directories"));

	return result;
}

std::string utilities::getHackPath()
{
	std::string res = {};
	try
	{
		fs::current_path(__DOCUMENTS);
		res = fs::absolute(XOR("Bartis_internal/csgo")).string();
	}
	catch (const fs::filesystem_error& err)
	{
		throw std::runtime_error(err.what());
	}
	return res;
}

std::string utilities::getTime()
{
	const auto now = std::chrono::system_clock::now();
	const auto time = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&time), XOR("%Y:%m:%d-%X"));
	return ss.str();
}

uintptr_t utilities::patternScan(const char* mod, const char* mask)
{
	MODULEINFO modInfo;
#ifndef _DEBUG
	LF(K32GetModuleInformation).cached()(LF(GetCurrentProcess).cached()(), LF(GetModuleHandleA).cached()(mod), &modInfo, sizeof(MODULEINFO));
#else
	LF(K32GetModuleInformation)(LF(GetCurrentProcess)(), LF(GetModuleHandleA)(mod), &modInfo, sizeof(MODULEINFO));
#endif		
	uintptr_t ranageStart = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
	std::istringstream iss{ mask };
	std::vector<std::string> parts{ std::istream_iterator<std::string>{ iss }, std::istream_iterator<std::string>{} };
	std::vector<std::optional<byte>> actualPattern = {};

	std::for_each(parts.cbegin(), parts.cend(), [&](const std::string& str) -> void
		{
			if (str == "?" || str == "??")
				actualPattern.emplace_back(std::nullopt);
			else
				actualPattern.emplace_back(static_cast<byte>(std::stoi(str, nullptr, 16)));
		});

	for (auto i = 0; i < modInfo.SizeOfImage; i++)
	{
		if (static auto check = [](byte* data, const std::vector<std::optional<byte>>& _mask) -> bool
			{
				for (const auto& _byte : _mask)
				{
					if (_byte && _byte.value() != *data)
						return false;
						data++;
				}
				return true;
			}; check(reinterpret_cast<byte*>(ranageStart + i), actualPattern))
		{
			return ranageStart + i;
		}
	}

	LF(MessageBoxA)(nullptr, XOR("Pattern scanning failed!"), XOR("Bartis hack"), MB_OK | MB_ICONWARNING);

	return 0;
}

bool utilities::getBox(Entity_t* ent, Box& box)
{
	// pretty much nothing to explain here, using engine and detect mins/maxs
	const auto col = ent->collideable();
	if (!col)
		return false;

	const auto& min = col->OBBMins();
	const auto& max = col->OBBMaxs();

	std::array<Vector, 8> points =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	if (!points.data())
		return false;

	const auto& tranFrame = ent->m_rgflCoordinateFrame();

	float left = std::numeric_limits<float>::max();
	float top = std::numeric_limits<float>::max();
	float right = -std::numeric_limits<float>::max();
	float bottom = -std::numeric_limits<float>::max();

	std::array<Vector, 8> screen = {};

	for (int i = 0; i < 8; i++)
	{
		if (!render::worldToScreen(math::transformVector(points.at(i), tranFrame), screen.at(i)))
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

size_t utilities::inByteOrder(const size_t netLong)
{
	std::array<byte, 4> data = {};
	memcpy(&data, &netLong, data.size());

	return (static_cast<size_t>(data.at(3)) << 0)
		| (static_cast<size_t>(data.at(2)) << 8)
		| (static_cast<size_t>(data.at(1)) << 16)
		| (static_cast<size_t>(data.at(0)) << 24);
}

std::string utilities::getKeyName(UINT virtualKey)
{
#ifdef _DEBUG
	UINT scanCode = MapVirtualKeyA(virtualKey, MAPVK_VK_TO_VSC);
#else
	UINT scanCode = LF(MapVirtualKeyA).cached()(virtualKey, MAPVK_VK_TO_VSC);
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

	char keyName[50];
#ifdef _DEBUG
	if (GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName)) != 0)
	{
		return keyName;
}
	else
	{
		return XOR("[None]");
	}
#else
	if (LF(GetKeyNameTextA).cached()(scanCode << 16, keyName, sizeof(keyName)) != 0)
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
	return std::move(result);
}
