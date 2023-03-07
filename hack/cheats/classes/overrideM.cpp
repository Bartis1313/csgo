#include <cheats/classes/overrideM.hpp>

void OverrideMouseType::runAll(float* x, float* y)
{
	for (auto el : m_hacksRun)
		el->run(x, y);
}