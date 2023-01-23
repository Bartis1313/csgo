#pragma once

#include <cheats/classes/frameStage.hpp>

class IConVar;

class AmbientLight : protected FrameStageType
{
public:
	constexpr AmbientLight() :
		FrameStageType{}
	{}

	void setButtonState(bool state) { m_buttonState = state; }
	void setPickerState(bool state) { m_pickerState = state; }
protected:
	virtual void run(int frame) override;
	virtual void init() override;
private:
	IConVar* m_ambientR;
	IConVar* m_ambientG;
	IConVar* m_ambientB;

	bool m_buttonState = false;
	bool m_pickerState = false;
};

GLOBAL_FEATURE(AmbientLight);