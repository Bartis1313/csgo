#include "renderView.hpp"

void RenderViewType::runAll(const CViewSetup& view)
{
	for (auto el : m_hacksRun)
		el->run(view);
}
