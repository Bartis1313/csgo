#pragma once

class ModulateColor
{
public:
	void run(void* thisptr, float* r, float* g, float* b);
};

[[maybe_unused]] inline auto g_ModulateColor = ModulateColor{};
