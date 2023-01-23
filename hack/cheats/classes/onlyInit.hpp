#pragma once

#include "base.hpp"

// when only init is needed, or we want to control execution by ourselves in special places
class OnlyInitType : protected BaseHack
{
public:
	constexpr OnlyInitType() :
		BaseHack{}
	{}
	// keep those methods to be nothing as default
	// mostly we don't need all of them
protected:
	virtual void reset() override {};
	virtual void load() override {};
	virtual void shutdown() override {};
	virtual void roundRestart() override {};
};