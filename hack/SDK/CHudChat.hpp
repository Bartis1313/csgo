#pragma once

#include "helpers/pad.hpp"

class CHudChat
{
	PAD(84);
	int m_timesOpened;
	PAD(8);
	bool m_isOpen;
	PAD(1088);
};