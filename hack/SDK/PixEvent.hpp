#pragma once

#include "IMatRenderContext.hpp"

#define PIX_VALVE_ORANGE 0xFFF5940F

class PIXEvent
{
public:
	PIXEvent(IMatRenderContext* pRenderContext, const char* szName, unsigned long color = PIX_VALVE_ORANGE)
	{
		m_pRenderContext = pRenderContext;
		//Assert(m_pRenderContext);
		//Assert(szName);
		m_pRenderContext->beginPIXEvent(color, szName);
	}
	~PIXEvent()
	{
		m_pRenderContext->endPIXEvent();
	}
private:
	IMatRenderContext* m_pRenderContext;
};

#ifndef CONCAT_IMPL
#define CONCAT_IMPL(x, y) x##y
#endif

#ifndef MACRO_CONCAT
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#endif

#define PIXEVENT PIXEvent MACRO_CONCAT(pix_, __COUNTER__)
