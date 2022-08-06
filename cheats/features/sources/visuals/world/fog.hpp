#pragma once

class FogController_t;

class FogController
{
public:
	void run(FogController_t* ent);
};

[[maybe_unused]] inline auto g_FogController = FogController{};
