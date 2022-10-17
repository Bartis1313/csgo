#pragma once

#include <classes/frameStage.hpp>
#include <classes/overViewMap.hpp>

class Entity_t;
class ClientClass;
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
private:
	struct WeatherFields_t
	{
		Entity_t* m_ent = nullptr;
		ClientClass* m_preciptation = nullptr;
		bool m_created = false;
	} m_weather;

	friend WeatherReset;
};

GLOBAL_FEATURE(WeatherController);

class WeatherReset : protected OverViewMapType
{
public:
	constexpr WeatherReset() :
		OverViewMapType{}
	{}

protected:
	virtual void run([[maybe_unused]] MapStruct* map) override;
};

GLOBAL_FEATURE(WeatherReset);
