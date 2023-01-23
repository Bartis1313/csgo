#pragma once

#include <cheats/classes/doPostScreen.hpp>

class IConVar;

class RemoveSky : protected DoPostScreenType
{
public:
	constexpr RemoveSky() :
		DoPostScreenType{}
	{}

protected:
	virtual void run();
	virtual void init() override;
private:
	IConVar* m_sky;
};

GLOBAL_FEATURE(RemoveSky);
