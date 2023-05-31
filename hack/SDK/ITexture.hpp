#pragma once

#include "helpers/pad.hpp"
#include "helpers/vfunc.hpp"

struct IDirect3DTexture9;

struct TextureHandle_t
{
	PAD(12);
	IDirect3DTexture9* m_texture;
};

class ITexture
{
public:
	VFUNC(int, getActualWidth, 3, (), (this));
	VFUNC(int, getActualHeight, 4, (), (this));
	void release()
	{
		if (!this)
		{
#ifdef _DEBUG
			printf("can't release texture, fatal err");
#endif
			return;
		}

		return vfunc::callVFunc<void, 11>(this);
	}
	PAD(80);
	TextureHandle_t** m_handle;
};