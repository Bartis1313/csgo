#pragma once

#include <SDK/IVModelInfo.hpp>
#include <SDK/vars.hpp>

enum FrameStage;
struct vcollide_t;
class IGameEvent;

namespace weatherController
{
	void init();
	void run(FrameStage stage);
	void reset();
	void shutdown();
	void implMenu();
	void handleSounds(IGameEvent* _event);
	void handleGroundMaterials();

	vcollide_t& getCollide();

	constexpr int uniqueModelIndex{ MAX_MODELS - 1 };
}