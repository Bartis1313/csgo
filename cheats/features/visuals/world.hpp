#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include "../../../SDK/math/Vector.hpp"
#include "../../../SDK/Color.hpp"

class Entity_t;
class Bomb_t;
class IConVar;
class IMaterial;
class CEffectData;
class FogController_t;
class EnvTonemapController_t;

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
private:
	void drawProjectiles(Entity_t* ent, const int id);
	void drawBomb(Entity_t* ent);
	void drawMolotov(Entity_t* ent);
	void drawSmoke(Entity_t* ent);
	void resetBomb() { m_bombEnt = nullptr; }
	Bomb_t* m_bombEnt;
	std::string m_whoPlanted = "";
	bool m_checkStateSlider = false;
	bool m_checkStateButton = false;
private:
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
};

inline World world;