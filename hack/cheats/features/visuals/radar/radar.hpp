#pragma once

#include <cheats/classes/renderableToPresent.hpp>
#include <cheats/classes/overViewMap.hpp>
#include <SDK/math/Vector.hpp>

#include <cstdint>
#include <string>

class RadarSizeHelper;
struct IDirect3DTexture9;
struct ImVec2;

// texture - https://www.unknowncheats.me/forum/counterstrike-global-offensive/317765-getting-map-overview-information-textured-radar.html
class Radar : protected RenderablePresentType
{
public:
	constexpr Radar() :
		RenderablePresentType{}
	{}

	[[nodiscard]] constexpr IDirect3DTexture9*& getTexture() { return m_mapTexture; }
	constexpr void setInited(bool val) { m_inited = val; }
	// will only work for vanilla maps
	bool manuallyInitTexture();
protected:
	virtual void draw() override;
private:
	[[nodiscard]] ImVec2 entToRadar(const Vec3& eye, const Vec3& angles, const Vec3& entPos, const float scale, bool clipRanges);
	struct MapPos
	{
		Vec2 m_pos;
		float m_scale;
	};
	MapPos getMapPos() const;
	void drawMap();
	void manuallyInitPos();

	Vec2 m_pos;
	float m_scale;
	Vec2 m_drawSize;
	Vec2 m_drawPos;
	bool m_inited;
	IDirect3DTexture9* m_mapTexture;

	friend RadarSizeHelper;
};

GLOBAL_FEATURE(Radar);

class RadarSizeHelper : public OverViewMapType
{
public:
	constexpr RadarSizeHelper() :
		OverViewMapType{}
	{}

protected:
	virtual void run(MapStruct* map);
};

GLOBAL_FEATURE(RadarSizeHelper);
