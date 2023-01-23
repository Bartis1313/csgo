#include <cheats/classes/overViewMap.hpp>

void OverViewMapType::runAll(MapStruct* map)
{
	for (auto el : m_hacksRun)
		el->run(map);
}