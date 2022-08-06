#pragma once

#include "../../../classes/frameStage.hpp"

class Entity_t;
class ClientClass;

class WeatherController : public FrameStageType
{
public:
	constexpr WeatherController() :
		FrameStageType{}
	{}

	virtual void init();
	virtual void run(int frame);
	// add to imgui
	void implMenu();
private:
	struct WeatherFields_t
	{
		Entity_t* m_ent = nullptr;
		ClientClass* m_preciptation = nullptr;
		bool m_created = false;
	} m_weather;
};

[[maybe_unused]] inline auto g_WeatherController = WeatherController{};
