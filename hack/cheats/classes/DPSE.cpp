#include "doPostScreen.hpp"

void DoPostScreenType::runAll()
{
	for (auto el : m_hacksRun)
		el->run();
}