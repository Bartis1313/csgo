#pragma once

#include "../../../classes/renderableToPresent.hpp"
#include "../../../classes/renderView.hpp"

#include "../../../SDK/math/Vector2D.hpp"

class CViewSetup;
class ITexture;
struct IDirect3DTexture9;
class MirrorCamDraw;

class MirrorCam : public RenderViewType
{
public:
	MirrorCam()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void run(const CViewSetup& view);
private:
	constexpr void setSize(const Vector2D& size) { m_size = size; }
	IDirect3DTexture9* getTexture() const;

	Vector2D m_size = { 1.0f, 1.0f };
	ITexture* m_texture;
	bool m_inited = false;

	friend MirrorCamDraw;
};

[[maybe_unused]] inline auto g_MirrorCam = MirrorCam{};

class MirrorCamDraw : public RenderablePresentType
{
public:
	MirrorCamDraw()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void draw();
};

[[maybe_unused]] inline auto g_MirrorCamDraw = MirrorCamDraw{};

