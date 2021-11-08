#include "misc.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../menu/vars.hpp"
#include "../../game.hpp"

constexpr int VK_VKEY = 0x56;

// TODO: rewrite this
void misc::thirdperson()
{
	if (!vars::bThirdp)
		return;

	if (!interfaces::engine->isInGame())
		return;

	if (!game::localPlayer || !game::localPlayer->isAlive())
		return;

	static bool bRun = false;
	if (LF(GetAsyncKeyState)(VK_VKEY) & 1)
		bRun = !bRun;

	
	interfaces::input->m_cameraInThirdPerson = bRun;
	interfaces::input->m_cameraOffset.z = 220.0f;
}