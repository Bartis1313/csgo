#include "viewRender.hpp"

void ViewRenderType::runAll(const CViewSetup& view)
{
	for (auto el : m_hacksRun)
		el->run(view);
}
