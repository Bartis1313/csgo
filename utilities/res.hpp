#pragma once

#include <string>
#include <vector>

struct IDirect3DTexture9;

class Resource final
{
public:
	Resource() = delete;
	~Resource();
	Resource(const std::string_view path);
	Resource(int resID, const std::string_view type);
	// imgui
	bool operator()() const { return m_texture; };
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