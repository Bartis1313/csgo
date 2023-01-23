#include "createMove.hpp"

void CreateMoveInPredictionType::runAll(CUserCmd* cmd)
{
	for (auto el : m_hacksRun)
		el->run(cmd);
}

void CreateMovePrePredictionType::runAll(CUserCmd* cmd)
{
	for (auto el : m_hacksRun)
		el->run(cmd);
}

void CreateMovePostPredictionType::runAll(CUserCmd* cmd)
{
	for (auto el : m_hacksRun)
		el->run(cmd);
}