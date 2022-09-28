#pragma once

#include <classes/renderableToPresent.hpp>
#include <classes/renderView.hpp>

#include <SDK/math/Vector.hpp>

class CViewSetup;
class ITexture;
struct IDirect3DTexture9;
class MirrorCamDraw;

class MirrorCam : public RenderViewType
{
public:
	constexpr MirrorCam() :
		RenderViewType{}
	{}

	virtual void init();
	virtual void run(const CViewSetup& view);
private:
	constexpr void setSize(const Vec2& size) { m_size = size; }
	IDirect3DTexture9* getTexture() const;

	Vec2 m_size = Vec2{ 1.0f, 1.0f };
	ITexture* m_texture;
	bool m_inited = false;

	friend MirrorCamDraw;
};

[[maybe_unused]] inline auto g_MirrorCam = MirrorCam{};

class MirrorCamDraw : public RenderablePresentType
{
public:
	constexpr MirrorCamDraw() :
		RenderablePresentType{}
	{}

	virtual void init();
	virtual void draw();
};

[[maybe_unused]] inline auto g_MirrorCamDraw = MirrorCamDraw{};

