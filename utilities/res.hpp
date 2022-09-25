#pragma once

#include <resources/resource.h>

#include <string>
#include <vector>

struct IDirect3DTexture9;

class Resource final
{
public:
	Resource(const std::string& path);
	Resource(int resID, const std::string_view type);
	Resource(void* data, size_t size);
	static void destroyAll();
	// imgui
	IDirect3DTexture9* getTexture() const { return m_texture; }
	// surface
	constexpr int getTextureID() const { return m_textureID; }
	constexpr int getWidth() const { return m_width; }
	constexpr int getHeight() const { return m_height; }
private:
	IDirect3DTexture9* m_texture;
	unsigned char* m_buffer;
	// surface
	int m_textureID = -1;
	int m_width;
	int m_height;

	inline static std::vector<Resource> m_resBuf;
};