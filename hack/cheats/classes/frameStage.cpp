#include "frameStage.hpp"

void FrameStageType::runAll(int frame)
{
	for (auto el : m_hacksRun)
		el->run(frame);
}