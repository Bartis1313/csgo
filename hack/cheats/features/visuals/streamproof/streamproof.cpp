#include "streamproof.hpp"

#include <SDK/IMaterialSystem.hpp>
#include <SDK/ITexture.hpp>
#include <SDK/interfaces/interfaces.hpp>

#include <cheats/game/globals.hpp>
#include <utilities/console/console.hpp>
#include <config/vars.hpp>

// we create an extra backbuffer to set our render target to it
void StreamProof::init()
{
	const auto backBuffer = memory::interfaces::matSys->getBackBufferFormat();
	memory::interfaces::matSys->beginRenderTargetAllocation();
	renderTarget = memory::interfaces::matSys->createNamedRenderTargetTextureEx(
		targetName.data(), 1, 1, RT_SIZE_FULL_FRAME_BUFFER,
		backBuffer, MATERIAL_RT_DEPTH_SHARED,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
		CREATERENDERTARGETFLAGS_HDR);
	memory::interfaces::matSys->endRenderTargetAllocation();

	console::debug("loaded streamproof texture {}", targetName);
}

void StreamProof::shutdown()
{
	if (renderTarget)
		renderTarget->release();
}

// here, we render this backbuffer texture
void StreamProof::beginPresent(IDirect3DDevice9* device)
{
	std::lock_guard lock{ mutex };

	if (!active)
		return;

	if (!renderTarget)
		return;

	const auto texture = getGameTexture();
	if (!texture)
	{
#ifdef _DEBUG
		printf("couldn't find texture\n");
#endif
		return;
	}

	IDirect3DSurface9* backbuffer{ };
	D3DSURFACE_DESC desc{ };
	device->GetRenderTarget(0, &backbuffer);
	backbuffer->GetDesc(&desc);

	// full sceenspace vertex resource
	const CUSTOMVERTEX vertexArray[]
	{
		{ 0.f, 0.f, 0.f, 1.0f, 0.f, 0.f }, // top-left vertex
		{ static_cast<float>(desc.Width), 0.0f, 0.0f, 1.0f, 1.0f, 0.0f }, // top-right vertex
		{ 0.0f, static_cast<float>(desc.Height), 0.0f, 1.0f, 0.0f, 1.0f }, // bottom-left vertex
		{ static_cast<float>(desc.Width), static_cast<float>(desc.Height), 0.0f, 1.0f, 1.0f, 1.0f } // bottom-right vertex
	};

	IDirect3DVertexBuffer9* vertexBuffer{ };
	// CUSTOMVERTEX is xyzrhw, and we have only 1 texture
	constexpr auto dxFVF{ D3DFVF_XYZRHW | D3DFVF_TEX1 };
	device->CreateVertexBuffer(sizeof(vertexArray), D3DUSAGE_WRITEONLY, dxFVF, D3DPOOL_DEFAULT, &vertexBuffer, nullptr);

	void* verticesData{ nullptr };
	vertexBuffer->Lock(0, 0, &verticesData, 0);
	std::memcpy(verticesData, vertexArray, sizeof(vertexArray));
	vertexBuffer->Unlock();

	// be careful with those flags, as you can see we don't clear anything. We let the game do that.
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
	//device->SetRenderState(D3DRS_ZENABLE, TRUE); // does not sync to texture

	// set this extra backbuffer texture to draw
	device->SetTexture(0, texture);

	device->SetFVF(dxFVF);
	device->SetStreamSource(0, vertexBuffer, 0, sizeof(CUSTOMVERTEX));

	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void StreamProof::endPresent()
{
	std::lock_guard lock{ mutex };

	if (!active)
		return;

	if (!renderTarget)
		return;

	// overriding depth may lead to fatal errors
	renderGameContext = memory::interfaces::matSys->getRenderContext();
	renderGameContext->pushRenderTargetAndViewport();
	renderGameContext->setRenderTarget(renderTarget);
	renderGameContext->clearColor4ub(0, 0, 0, 255); // color clears weird, looks like some var is not used for depth
	renderGameContext->clearBuffers(true, true, true); // may crash on other vars
	renderGameContext->popRenderTargetAndViewport();
	renderGameContext->release();
}

// here we tell the game to use our render target during the drawing proccess
void StreamProof::beginMaterialHook()
{
	if (!active)
		return;

	if (!renderTarget)
		return;

	mutex.lock();

	renderGameContext = memory::interfaces::matSys->getRenderContext();
	renderGameContext->pushRenderTargetAndViewport();
	renderGameContext->setRenderTarget(renderTarget);
}

// here we tell the game to manually backup our render target, so we don't have artifacts
void StreamProof::endMaterialHook()
{
	if (!active)
		return;

	if (!renderTarget)
		return;

	renderGameContext->popRenderTargetAndViewport();
	renderGameContext->release();

	mutex.unlock();
}

IDirect3DTexture9* StreamProof::getGameTexture() const
{
	return renderTarget->m_handle[0]->m_texture;
}