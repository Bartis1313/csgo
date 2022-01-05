#pragma once
#include "../utilities/pad.hpp"
#include "../utilities/vfunc.hpp"

class Input
{
public:
	PAD(173);
	bool m_cameraInThirdPerson;
	PAD(1);
	Vector m_cameraOffset;
};