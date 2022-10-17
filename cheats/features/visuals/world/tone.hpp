#pragma once

#include <classes/frameStage.hpp>

class ToneController : protected FrameStageType
{
public:
	constexpr ToneController() :
		FrameStageType{}
	{}

	void setStateSlider(bool state) { m_checkStateSlider = state; }
	void setStateButton(bool state) { m_checkStateButton = state; }
protected:
	virtual void run(int frame) override;
private:
	bool m_checkStateSlider = false;
	bool m_checkStateButton = false;
};

GLOBAL_FEATURE(ToneController);
