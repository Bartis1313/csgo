#include "screen2dEffects.hpp"

void Screen2DEffectsType::runAll()
{
	for (auto el : m_hacksRun)
		el->run();
}
