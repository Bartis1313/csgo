#pragma once

struct MapStruct;
struct IDirect3DTexture9;

// texture - https://www.unknowncheats.me/forum/counterstrike-global-offensive/317765-getting-map-overview-information-textured-radar.html
namespace radar
{
	void draw();
	bool manuallyInitTexture();
	void gatherMap(MapStruct* map);

	inline IDirect3DTexture9* mapTexture{ nullptr };
};
