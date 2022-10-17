#pragma once

#include "base.hpp"

#include <vector>

struct MapStruct;

class OverViewMapType : protected BaseHack
{
public:
	constexpr OverViewMapType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	static void runAll(MapStruct* map);
protected:
	virtual void run(MapStruct* map) = 0;
	// keep those methods to be nothing as default
	// mostly we don't need all of them
	virtual void init() override {};
	virtual void reset() override {};
	virtual void shutdown() override {};
private:
	inline static std::vector<OverViewMapType*> m_hacksRun;
};