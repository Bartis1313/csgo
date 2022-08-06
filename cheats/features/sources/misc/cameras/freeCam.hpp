#pragma once

#include "../../../classes/overrideView.hpp"

#include "../../../SDK/math/Vector.hpp"

class CViewSetup;

class Freecam : public OverrideViewType
{
public:
	Freecam()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	// need to set angles to remove view effect
	virtual void run(CViewSetup* view);
	_NODISCARD constexpr bool isInCam() const { return m_inCam; }
private:
	bool m_inCam = false;
	Vector m_view;
};

[[maybe_unused]] inline auto g_Freecam = Freecam{};

#include "../../../classes/renderableToPresent.hpp"

class FreecamDraw : public RenderablePresentType
{
public:
	FreecamDraw()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void draw();
};

[[maybe_unused]] inline auto g_FreecamDraw = FreecamDraw{};