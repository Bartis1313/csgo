#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <array>

#include "../../../SDK/math/Vector.hpp"
#include "../../../SDK/Color.hpp"

class Entity_t;
class Bomb_t;
class IConVar;
class IMaterial;
class CEffectData;
class FogController_t;
class EnvTonemapController_t;
class ClientClass;
class CViewSetup;

struct hitStructLocal_t
{
	Vector m_start;
	Vector m_end;
	float m_expire;
};

class World final
{
public:
	void drawMisc();

	void skyboxLoad(int stage, bool isShotdown);
	void removeSky(bool isShotdown);
	void modulateWorld(void* thisptr, float* r, float* g, float* b, bool isShutdown);
	void drawZeusRange();
	void drawMovementTrail();
	void clientSideImpacts();
	void localImpacts();
	void pushLocalImpacts(const hitStructLocal_t& hit);
	void removeBloodSpray(int frame);
	void removeSmoke(int frame);
	void drawCustomSmoke(const Vector& pos, float radius);
	void drawBombOverlay();
	_NODISCARD constexpr std::vector<std::string> getAllCustomSkyBoxes() const { return  m_allCustomSkyboxes; }
	void reloadCustomSkyboxes();
	bool initSkyboxes();
	void runFog(FogController_t* ent);
	void drawEffects();
	void initEffects();
	void runToneController(EnvTonemapController_t* ent);
	constexpr void setBombEnt(Bomb_t* ent) { m_bombEnt = ent; }
	constexpr void setWhoPlanted(const std::string& name) { m_whoPlanted = name; }
	constexpr void setCheckStateSlider(bool state) { m_checkStateSlider = state; }
	constexpr void setCheckStateButton(bool state) { m_checkStateButton = state; }
	void weather(int stage);
	// manual helper for map change
	constexpr void setWeatherState(bool state) { m_weather.m_created = state; }
	void implMenu() const;
	bool doImageSpaceMotionBlur(CViewSetup* view);
	void renderMotionBlur();
private:
	void drawProjectiles(Entity_t* ent, const int id);
	void drawBomb(Entity_t* ent);
	void drawMolotov(Entity_t* ent);
	void drawSmoke(Entity_t* ent);
	void resetBomb() { m_bombEnt = nullptr; }
	Bomb_t* m_bombEnt = nullptr;
	std::string m_whoPlanted = "";
	bool m_checkStateSlider = false;
	bool m_checkStateButton = false;
	bool checkCustomSkybox();
	IConVar* m_oldSky = nullptr;
	struct Trail_t
	{
		Vector m_pos;
		float m_expire;
		Color m_col;
	};
	std::vector<Trail_t> m_trails;
	struct clientHitVerify_t
	{
		Vector m_pos;
		float m_time;
		float m_expire;
	};
	struct hitStruct_t
	{
		Vector m_pos;
		float m_expire;
	};
	std::vector<hitStruct_t> m_hitsClientSide;
	std::vector<hitStructLocal_t> m_hitsLocal;
	std::filesystem::path m_pathCustomSkybox;
	std::vector<std::string> m_allCustomSkyboxes;

	std::vector<std::pair<IMaterial*, bool>> m_materials;

	struct WeatherFields_t
	{
		Entity_t* m_ent = nullptr;
		ClientClass* m_preciptation = nullptr;
		bool m_created = false;
	} m_weather;

	struct MotionBlurHistory_t
	{
		MotionBlurHistory_t() :
			m_lastTimeUpdate{ 0.0f }, m_previousPitch{ 0.0f }, m_previousYaw{ 0.0f },
			m_previousPositon{ Vector{} }, m_noRotationalMotionBlurUntil{ 0.0f }
		{}

		float m_lastTimeUpdate;
		float m_previousPitch;
		float m_previousYaw;
		Vector m_previousPositon;
		float m_noRotationalMotionBlurUntil;
	} m_motionHistory;

	std::array<float, 4> m_motionBlurValues = { 0.0f, 0.0f, 0.0f, 0.0f };
	std::array<float, 4> m_motionBlurViewportValues = { 0.0f, 0.0f, 0.0f, 0.0f };
};

inline World world;