#pragma once

#include <classes/doPostScreen.hpp>

class IConVar;

class RemoveSky : public DoPostScreenType
{
public:
	constexpr RemoveSky() :
		DoPostScreenType{}
	{}

	virtual void init();
	virtual void run();
private:
	IConVar* m_sky;
};

[[maybe_unused]] inline auto g_RemoveSky = RemoveSky{};
