#pragma once

#include <classes/renderableToPresent.hpp>
#include <classes/overViewMap.hpp>
#include <SDK/math/Vector2D.hpp>
#include <SDK/math/Vector.hpp>

#include <cstdint>
#include <string>

class RadarSizeHelper;
struct IDirect3DTexture9;

// texture - https://www.unknowncheats.me/forum/counterstrike-global-offensive/317765-getting-map-overview-information-textured-radar.html
class Radar : public RenderablePresentType
{
public:
	constexpr Radar() :
		RenderablePresentType{}
	{}

	virtual void init();
	virtual void draw();
	_NODISCARD constexpr IDirect3DTexture9*& getTexture() { return m_mapTexture; }
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
	IDirect3DTexture9* m_mapTexture;

	friend RadarSizeHelper;
};

[[maybe_unused]] inline auto g_Radar = Radar{};

class RadarSizeHelper : public OverViewMapType
{
public:
	constexpr RadarSizeHelper() :
		OverViewMapType{}
	{}

	virtual void init();
	virtual void run(MapStruct* map);
};

[[maybe_unused]] inline auto g_RadarSizeHelper = RadarSizeHelper{};
