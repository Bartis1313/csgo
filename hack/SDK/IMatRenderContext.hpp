#pragma once

#include "helpers/vfunc.hpp"

class ITexture;
class IMaterial;
struct ShaderStencilState_t;

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
    VFUNC(void, overrideDepthEnable, 80, (bool enable, bool depthWrite, bool depthTestEnable = true), (this, enable, depthWrite, depthTestEnable));
    VFUNC(void, setStencilState, 128, (const ShaderStencilState_t& state), (this, std::cref(state)));
    VFUNC(void, beginPIXEvent, 144, (unsigned long color, const char* name), (this, color, name));
    VFUNC(void, endPIXEvent, 145, (), (this));
    VFUNC(void, viewport, 40, (int x, int y, int width, int height), (this, x, y, width, height));
    VFUNC(void, getViewport, 41, (int& x, int& y, int& width, int& height), (this, std::ref(x), std::ref(y), std::ref(width), std::ref(height)));
    VFUNC(void, clearColor3ub, 78, (unsigned char r, unsigned char g, unsigned char b), (this, r, g, b));
    VFUNC(void, depthRange, 11, (float zNear, float zFar), (this, zNear, zFar));
    VFUNC(void, clearBuffers, 12, (bool clearColor, bool clearDepth, bool clearStencil = false), (this, clearColor, clearDepth, clearStencil));

    // custom
    void drawScreenEffectMaterial(IMaterial* material);
};