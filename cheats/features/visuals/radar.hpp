#pragma once

#include <cstdint>
#include <string>

#include "../../../SDK/math/Vector2D.hpp"

struct Vector;
struct IDirect3DTexture9;
struct MapStruct;

// texture - https://www.unknowncheats.me/forum/counterstrike-global-offensive/317765-getting-map-overview-information-textured-radar.html
class Radar final
{
public:
	void initRetAddr();
	void run();
	void onInit(MapStruct* map);
	_NODISCARD constexpr IDirect3DTexture9*& getTexture() { return m_mapTexture; }
	_NODISCARD constexpr uintptr_t* getRetAddr() { return m_retAddr; }
	constexpr void setInited(bool val) { m_inited = val; }
	// will only work for vanilla maps
	bool manuallyInitTexture();
private:
	_NODISCARD Vector2D entToRadar(const Vector& eye, const Vector& angles, const Vector& entPos, const Vector2D& pos, const Vector2D& size, const float scale, bool clipRanges);
	struct MapPos
	{
		Vector2D m_pos;
		float m_scale;
	};
	MapPos getMapPos() const;
	void drawMap();
	void manuallyInitPos();

	Vector2D m_pos;
	float m_scale;
	Vector2D m_drawSize;
	Vector2D m_drawPos;
	bool m_inited;
	uintptr_t* m_retAddr;
	IDirect3DTexture9* m_mapTexture;
};

inline Radar radar;