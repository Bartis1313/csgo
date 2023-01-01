#include "IPrediction.hpp"

#include <gamememory/memory.hpp>

void IPrediction::shutdownPredictables()
{
	memory::shutdownPredicatbles()();
}

int IPrediction::reinitPredictables()
{
	return memory::reinitPredicatbles()();
}