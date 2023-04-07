#pragma once

#include <cheats/classes/frameStage.hpp>
#include <cheats/classes/overViewMap.hpp>

class Entity_t;
class WeatherReset;
struct MapStruct;

class WeatherController : protected FrameStageType
{
public:
	constexpr WeatherController() :
		FrameStageType{}
	{}

	// add to imgui
	void implMenu();
protected:
	virtual void run(int frame) override;
	virtual void reset() override;
private:
	struct WeatherFields_t
	{
		Entity_t* m_ent = nullptr;
		Entity_t* m_networkable = nullptr;
		bool m_created = false;
	} m_weather;

	friend WeatherReset;
};

GLOBAL_FEATURE(WeatherController);
