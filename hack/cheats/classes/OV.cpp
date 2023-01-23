#include <cheats/classes/overrideView.hpp>

void OverrideViewType::runAll(CViewSetup* view)
{
	for (auto el : m_hacksRun)
		el->run(view);
}