#include "renderableToSurface.hpp"

void RenderableSurfaceType::runAll()
{
	for (auto el : m_hacksRun)
		el->draw();
}