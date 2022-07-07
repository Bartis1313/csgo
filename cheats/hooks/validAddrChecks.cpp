#include "hooks.hpp"
#include "../../config/vars.hpp"

char __fastcall hooks::clientValidAddr::hooked(FAST_ARGS, const char* lpModuleName)
{
	return 1;
}

char __fastcall hooks::engineValidAddr::hooked(FAST_ARGS, const char* lpModuleName)
{
	return 1;
}

char __fastcall hooks::studioRenderValidAddr::hooked(FAST_ARGS, const char* lpModuleName)
{
	return 1;
}

char __fastcall hooks::materialSystemValidAddr::hooked(FAST_ARGS, const char* lpModuleName)
{
	return 1;
}

bool __fastcall hooks::isUsingStaticPropDebugModes::hooked(FAST_ARGS)
{
	return config.get<bool>(vars.bModulateColor); // might check ret addr, but pretty useless imho
}

void __fastcall hooks::doExtraBonesProcessing::hooked(FAST_ARGS, void* hdr, void* pos, void* q, const Matrix3x4& matrix, void* computed, void* contex)
{
	return;
}

int __fastcall hooks::getUnverifiedFileHashes::hooked(FAST_ARGS, int maxFiles)
{
	return 0;
}

int __fastcall hooks::unkFileCheck::hooked(FAST_ARGS)
{
	return 1; // 2 is kick
}

//#include "../../SDK/Input.hpp"
//#include "../../SDK/interfaces/interfaces.hpp"
#include "../../SDK/structs/IDXandPaterrns.hpp"

bool __fastcall hooks::sv_cheats::hooked(FAST_ARGS)
{
	/*if (interfaces::input->m_cameraInThirdPerson)
		return true;*/

	static auto camThink = utilities::patternScan(CLIENT_DLL, CAM_THINK);

	if (_ReturnAddress() == reinterpret_cast<void*>(camThink) && config.get<bool>(vars.bThirdp))
		return true;

	return original(thisptr);
}