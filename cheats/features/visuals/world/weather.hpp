#pragma once

#include <classes/frameStage.hpp>
#include <classes/overViewMap.hpp>

class Entity_t;
class ClientClass;
class WeatherReset;
struct MapStruct;

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

	friend WeatherReset;
};

[[maybe_unused]] inline auto g_WeatherController = WeatherController{};

class WeatherReset : public OverViewMapType
{
public:
	constexpr WeatherReset() :
		OverViewMapType{}
	{}

	virtual void init();
	virtual void run([[maybe_unused]] MapStruct* map);
};

[[maybe_unused]] inline auto g_WeatherReset = WeatherReset{};
