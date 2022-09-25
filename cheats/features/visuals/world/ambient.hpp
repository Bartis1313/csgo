#pragma once

#include <classes/frameStage.hpp>

class IConVar;

class AmbientLight : public FrameStageType
{
public:
	constexpr AmbientLight() :
		FrameStageType{}
	{}

	virtual void init();
	virtual void run(int frame);

	void setButtonState(bool state) { m_buttonState = state; }
	void setPickerState(bool state) { m_pickerState = state; }
private:
	IConVar* m_ambientR;
	IConVar* m_ambientG;
	IConVar* m_ambientB;

	bool m_buttonState = false;
	bool m_pickerState = false;
};

[[maybe_unused]] inline auto g_AmbientLight = AmbientLight{};