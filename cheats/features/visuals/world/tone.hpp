#pragma once

#include <classes/frameStage.hpp>

class ToneController : public FrameStageType
{
public:
	constexpr ToneController() :
		FrameStageType{}
	{}

	virtual void init();
	virtual void run(int frame);
	void setStateSlider(bool state) { m_checkStateSlider = state; }
	void setStateButton(bool state) { m_checkStateButton = state; }
private:
	bool m_checkStateSlider = false;
	bool m_checkStateButton = false;
};

[[maybe_unused]] inline auto g_ToneController = ToneController{};
