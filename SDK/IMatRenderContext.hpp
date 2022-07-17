#pragma once

#include "../utilities/vfunc.hpp"

class ITexture;
class IMaterial;

class IMatRenderContext
{
public:
    VFUNC(int, release, 1, (), (this));
    VFUNC(void, setRenderTarget, 6, (ITexture* texture), (this, texture));
    VFUNC(void, pushRenderTargetAndViewport, 119, (), (this));
    VFUNC(void, popRenderTargetAndViewport, 120, (), (this));
    VFUNC(void, drawScreenSpaceRectangle, 114,
        (IMaterial* material,
        int destX, int destY, int width, int height,
        float srcTextureX0, float srcTextureY0, float srcTextureX1, float srcTextureY1,
        int srcTextureWidth, int srcTextureHeight,
        void* clientRenderable = nullptr, int nXDice = 1, int nYDice = 1
        ), (this, material, destX, destY, width, height,
            srcTextureX0, srcTextureY0, srcTextureX1, srcTextureY1,
            srcTextureWidth, srcTextureHeight, clientRenderable, nXDice, nYDice));
};