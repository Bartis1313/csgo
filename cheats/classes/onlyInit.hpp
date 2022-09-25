#pragma once

#include "base.hpp"

// when only init is needed, or we want to control execution by ourselves in special places
class OnlyInitType : public BaseHack
{
public:
	constexpr OnlyInitType() :
		BaseHack{}
	{}
};