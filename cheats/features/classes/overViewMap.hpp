#pragma once

#include "../base.hpp"

#include <vector>

struct MapStruct;

class OverViewMapType : public BaseHack
{
public:
	virtual void init() {};
	virtual void run(MapStruct* map) {};
	static void runAll(MapStruct* map);
protected:
	inline static std::vector<OverViewMapType*> m_hacksRun;
};