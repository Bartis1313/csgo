#include "utilities.hpp"

#include "tools/tools.hpp"

#include <Windows.h>
#include <chrono>

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

std::string utilities::getKeyName(const uint32_t virtualKey)
{
	uint32_t scanCode = LI_FN(MapVirtualKeyA).cached()(virtualKey, MAPVK_VK_TO_VSC);

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

	if (char keyName[50]; LI_FN_CACHED(GetKeyNameTextA)(scanCode << 16, keyName, sizeof(keyName)) != 0)
		return keyName;
	else
		return XOR("[None]");
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
	return LI_FN_CACHED(GetAsyncKeyState)(vKey);
}

std::string utilities::u8toStr(const std::u8string& u8str)
{
	return std::string{ u8str.cbegin(), u8str.cend() };
}