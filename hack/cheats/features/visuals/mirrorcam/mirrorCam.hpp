#pragma once

#include <cheats/classes/renderableToPresent.hpp>
#include <cheats/classes/viewRender.hpp>
#include <cheats/classes/wndProcKeyHandler.hpp>

#include <SDK/math/Vector.hpp>

class CViewSetup;
class ITexture;
struct IDirect3DTexture9;
class MirrorCamDraw;

class MirrorCam : protected ViewRenderType, protected WndProcKeyHandler
{
public:
	constexpr MirrorCam() :
		ViewRenderType{},
		WndProcKeyHandler{}
	{}

protected:
	virtual void run(const CViewSetup& view) override;
	virtual void updateKeys() override;
private:
	constexpr void setSize(const Vec2& size) { m_size = size; }
	IDirect3DTexture9* getTexture() const;
	void initTexture();

	Vec2 m_size{ 1.0f, 1.0f };
	ITexture* m_texture{ };

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
