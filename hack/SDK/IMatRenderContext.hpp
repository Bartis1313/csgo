#pragma once

#include "helpers/vfunc.hpp"
#include "stencil.hpp"

#include "math/Rect.hpp"

class ITexture;
class IMaterial;

enum MaterialMatrixMode_t
{
    MATERIAL_VIEW = 0,
    MATERIAL_PROJECTION,

    MATERIAL_MATRIX_UNUSED0,
    MATERIAL_MATRIX_UNUSED1,
    MATERIAL_MATRIX_UNUSED2,
    MATERIAL_MATRIX_UNUSED3,
    MATERIAL_MATRIX_UNUSED4,
    MATERIAL_MATRIX_UNUSED5,
    MATERIAL_MATRIX_UNUSED6,
    MATERIAL_MATRIX_UNUSED7,

    MATERIAL_MODEL,

    // Total number of matrices
    NUM_MATRIX_MODES = MATERIAL_MODEL + 1,
};

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
    VFUNC(void, setRenderTargetEx, 130, (int id, ITexture* texture), (this, id, texture));
    VFUNC(void, beginPIXEvent, 144, (unsigned long color, const char* name), (this, color, name));
    VFUNC(void, endPIXEvent, 145, (), (this));
    VFUNC(void, viewport, 40, (int x, int y, int width, int height), (this, x, y, width, height));
    VFUNC(void, getViewport, 41, (int& x, int& y, int& width, int& height), (this, std::ref(x), std::ref(y), std::ref(width), std::ref(height)));
    VFUNC(void, clearColor3ub, 78, (unsigned char r, unsigned char g, unsigned char b), (this, r, g, b));
    VFUNC(void, clearColor4ub, 79, (unsigned char r, unsigned char g, unsigned char b, unsigned char a), (this, r, g, b, a));
    VFUNC(void, depthRange, 11, (float zNear, float zFar), (this, zNear, zFar));
    VFUNC(void, clearBuffers, 12, (bool clearColor, bool clearDepth, bool clearStencil = false), (this, clearColor, clearDepth, clearStencil));
    VFUNC(void, overrideAlphaWriteEnable, 195, (bool overrideEnable, bool alphaWriteEnable), (this, overrideEnable, alphaWriteEnable));
    VFUNC(void, overrideColorWriteEnable, 196, (bool overrideEnable, bool alphaWriteEnable), (this, overrideEnable, alphaWriteEnable));
    VFUNC(void, copyRenderTargetToTexture, 19, (ITexture* texture), (this, texture));
    VFUNC(void, bind, 9, (IMaterial* material, void* proxy = nullptr), (this, material, proxy));
    VFUNC(void, matrixMode, 22, (MaterialMatrixMode_t mode), (this, mode));
    VFUNC(void, pushMatrix, 23, (), (this));
    VFUNC(void, popMatrix, 24, (), (this));
    VFUNC(void, loadIdentity, 33, (), (this));
    VFUNC(void, ortho, 34, (double left, double top, double right, double bottom, double zNear, double zFar), (this, left, top, right, bottom, zNear, zFar));
    VFUNC(void, scale, 39, (float x, float y, float z), (this, x, y, z));
    VFUNC(void, copyRenderTargetToTextureEx, 123, (int renderTargetID, ITexture* texture, Rect_t* srcRect, Rect_t* dstRect = nullptr), 
        (this, renderTargetID, texture, srcRect, dstRect));

    void disableStencil()
    {
        ShaderStencilState_t stencilState;
        stencilState.m_bEnable = false;
        this->setStencilState(stencilState);
    }

    // custom
    void drawScreenEffectMaterial(IMaterial* material);
};