#include "ambient.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/Enums.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <utilities/tools/tools.hpp>
#include <config/vars.hpp>
#include <cheats/game/globals.hpp>

#include <cheats/hooks/frameStageNotify.hpp>

namespace
{
	struct AmbientHandler : hooks::FrameStageNotify
	{
		AmbientHandler()
		{
			this->registerInit(ambientLight::init);
			this->registerRun(ambientLight::run);
			this->registerShutdown(ambientLight::shutdown);
		}
	} ambientHandler;
}

namespace ambientLight
{
	IConVar* ambientR;
	IConVar* ambientG;
	IConVar* ambientB;
}

// C7 44 24 ? ? ? ? ? 8B 84 24 ? ? ? ? in sdsshader_dx9 looks to be an array of those
void ambientLight::init()
{
	ambientR = memory::interfaces::cvar->findVar("mat_ambient_light_r");
	ambientG = memory::interfaces::cvar->findVar("mat_ambient_light_g");
	ambientB = memory::interfaces::cvar->findVar("mat_ambient_light_b");
}

void ambientLight::run(FrameStage stage)
{
	if (stage != FRAME_RENDER_START)
		return;

	// cant use env from memory, it wont differ just by changing fields

	if (auto cfg = vars::visuals->world->ambient->enabled; cfg && (buttonState || pickerState))
	{
		auto cfgCol = vars::visuals->world->ambient->color();

		ambientR->setValue(cfgCol.r());
		ambientG->setValue(cfgCol.g());
		ambientB->setValue(cfgCol.b());

	}
	else if (!cfg && buttonState)
	{
		ambientR->setValue(0.0f);
		ambientG->setValue(0.0f);
		ambientB->setValue(0.0f);
	}
}

void ambientLight::shutdown()
{
	ambientR->setValue(0.0f);
	ambientG->setValue(0.0f);
	ambientB->setValue(0.0f);
}