#include "utilities.hpp"
#include <Psapi.h>
#include <filesystem>
#include <ShlObj_core.h>
#include "renderer/renderer.hpp"
#include "../SDK/structs/Entity.hpp"

#undef max
#undef min

namespace fs = std::filesystem;

#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

namespace utilities
{
	std::string getFolder()
	{
		CHAR documents[MAX_PATH];
		HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents);

		return std::string(documents);
	}

	bool prepareDirectories()
	{
		fs::current_path(__DOCUMENTS);
		//CreateDirectoryA(std::string(__DOCUMENTS + std::string("\\") + std::string("Bartis internal")).c_str(), NULL);
		fs::create_directories(XOR("Bartis_internal/csgo"));

		return true;
	}

	std::string getHackPath()
	{
		fs::current_path(__DOCUMENTS);
		return fs::absolute(XOR("Bartis_internal/csgo")).string();
	}

	std::string getTime()
	{
		const auto now = std::chrono::system_clock::now();
		const auto time = std::chrono::system_clock::to_time_t(now);
		std::stringstream ss;
		ss << std::put_time(std::localtime(&time), XOR("%Y:%m:%d-%X"));
		return ss.str();
	}

	uintptr_t patternScan(const char* mod, const char* mask)
	{
		auto m_mask = mask;
		uintptr_t first = 0;

		MODULEINFO modInfo;
#ifndef _DEBUG
		LF(K32GetModuleInformation).cached()(LF(GetCurrentProcess).cached()(), LF(GetModuleHandleA).cached()(mod), &modInfo, sizeof(MODULEINFO));
#else
		LF(K32GetModuleInformation)(LF(GetCurrentProcess)(), LF(GetModuleHandleA)(mod), &modInfo, sizeof(MODULEINFO));
#endif		
		uintptr_t ranageStart = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
		uintptr_t rangeEnd = ranageStart + modInfo.SizeOfImage;

		for (auto curAddr = ranageStart; curAddr < rangeEnd; curAddr++)
		{
			if (*(PBYTE)m_mask == '\?' || *(BYTE*)curAddr == getByte(m_mask))
			{
				if (!*m_mask)
					return first;

				if (!first)
					first = curAddr;

				if (!m_mask[2])
					return first;

				if (*(PWORD)m_mask == '\?\?' || *(PBYTE)m_mask != '\?')
					m_mask += 3;
				else
					m_mask += 2;
			}
			else
			{
				m_mask = mask;
				first = 0;

				if (first != 0)
					curAddr = first;
				m_mask = mask;
				first = 0;
			}
		}
		LF(MessageBoxA)(nullptr, XOR("Pattern scanning failed!"), XOR("Bartis hack"), MB_OK | MB_ICONWARNING);

		return 0;
	}

	bool getBox(Entity_t* ent, Box& box)
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
			if (!render::WorldToScreen(math::transformVector(points.at(i), tranFrame), screen.at(i)))
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
}