#include "test.hpp"

#include <SDK/Enums.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/CUtlVector.hpp>

#include <gamememory/memory.hpp>
#include <cheats/hooks/frameStageNotify.hpp>

// place anything you want to test

namespace
{
	struct WeatherHandler : hooks::FrameStageNotify
	{
		WeatherHandler()
		{
			this->registerRun(test::run);
		}
	} weatherHandler;
}

void test::run(FrameStage stage)
{
	
}