#pragma once

class EnvTonemapController_t;

class ToneController
{
public:
	void run(EnvTonemapController_t* ent);
	void setStateSlider(bool state) { m_checkStateSlider = state; }
	void setStateButton(bool state) { m_checkStateButton = state; }
private:
	bool m_checkStateSlider = false;
	bool m_checkStateButton = false;
};

[[maybe_unused]] inline auto g_ToneController = ToneController{};
