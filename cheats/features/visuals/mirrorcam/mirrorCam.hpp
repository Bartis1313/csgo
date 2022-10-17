#pragma once

#include <classes/renderableToPresent.hpp>
#include <classes/renderView.hpp>

#include <SDK/math/Vector.hpp>

class CViewSetup;
class ITexture;
struct IDirect3DTexture9;
class MirrorCamDraw;

class MirrorCam : protected RenderViewType
{
public:
	constexpr MirrorCam() :
		RenderViewType{}
	{}

protected:
	virtual void run(const CViewSetup& view) override;
	virtual void init() override;
private:
	constexpr void setSize(const Vec2& size) { m_size = size; }
	IDirect3DTexture9* getTexture() const;

	Vec2 m_size = Vec2{ 1.0f, 1.0f };
	ITexture* m_texture;
	bool m_inited = false;

	friend MirrorCamDraw;
};

GLOBAL_FEATURE(MirrorCam);

class MirrorCamDraw : protected RenderablePresentType
{
public:
	constexpr MirrorCamDraw() :
		RenderablePresentType{}
	{}

protected:
	virtual void draw() override;
};

GLOBAL_FEATURE(MirrorCamDraw);
