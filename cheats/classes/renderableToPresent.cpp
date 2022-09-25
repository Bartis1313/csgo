#include "renderableToPresent.hpp"

void RenderablePresentType::runAll()
{
	for (auto el : m_hacksRun)
		el->draw();
}