#pragma once

#include "../../../SDK/math/Vector2D.hpp"

class CViewSetup;
class ITexture;
struct IDirect3DTexture9;

class MirrorCam
{
public:
	void init();
	void draw();
	void renderCustomView(const CViewSetup& view);
	constexpr void setSize(const Vector2D& size) { m_size = size; }
private:
	Vector2D m_size = { 1.0f, 1.0f };
	ITexture* m_texture;
	// to draw
	IDirect3DTexture9* getTexture() const;

	bool m_inited = false;
};

inline MirrorCam mCam;
