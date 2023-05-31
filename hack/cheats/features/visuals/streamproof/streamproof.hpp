#pragma once

#include <d3d9.h>
#include <SDK/IMatRenderContext.hpp>

#include <mutex>
#include <string_view>
#include <functional>

// works somewhat, but not really as game wants to
// I leave this for anyone who reads this to spend their pain on game's mesh builders queue'd rendering
// if you want to use this for surface - don't, you will cause massive threading deadlocks
// TODO: add handlers, this is currently all manual
class StreamProof final
{
public:
	StreamProof() = default;
	// target name better be unique
	StreamProof(const std::string_view targetName)
		: targetName{ targetName }
	{}

	// run in game thread once
	void init();
	void shutdown();
	// force texture draw in the present
	void beginPresent(IDirect3DDevice9* device);
	// cleanup with game's functions
	void endPresent();
	// before the original hook return
	void beginMaterialHook();
	// call this AFTER original has been called
	// store the original and return later
	void endMaterialHook();
	void setActive(const bool var) { active = var; }

	ITexture* getRenderTarget() const { return renderTarget; }
	IDirect3DTexture9* getGameTexture() const;
private:
	struct CUSTOMVERTEX
	{
		FLOAT x, y, z; // position coordinates
		FLOAT rhw; // reciprocal of homogeneous W
		// D3DCOLOR color; // vertex color
		FLOAT tu, tv; // texture coordinates
	};

	bool active{ };
	std::mutex mutex{ };
	std::string_view targetName{ };
	IMatRenderContext* renderGameContext{ };
	ITexture* renderTarget{ };
};