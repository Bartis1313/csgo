#include "res.hpp"

#include <Windows.h>
#include <d3dx9.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../dependencies/stb_image.h"
#include "../dependencies/ImGui/imgui_impl_dx9.h"

#include "../cheats/globals.hpp"
#include "utilities.hpp"
#include "../utilities/renderer/renderer.hpp"
#include "../SDK/interfaces/interfaces.hpp"
#include "../utilities/console/console.hpp"

// not going to use D3DXCreateTextureFromFileA or D3DXCreateTextureFromResourceA
// first - this API is very old and second D3DXCreateTextureFromResourceA had problems and my resource was forced to be RCDATA type instead of generated one to work.

Resource::Resource(const std::string_view path)
{
	if (path.empty())
		return;

    stbi_set_flip_vertically_on_load_thread(false);
    m_buffer = stbi_load(path.data(), &m_width, &m_height, nullptr, 4);
    // imgui
    m_texture = reinterpret_cast<IDirect3DTexture9*>(ImGui_CreateTexture(m_buffer, m_width, m_height));
    surfaceRender.initNewTexture(m_textureID, m_buffer, m_width, m_height);
    stbi_image_free(m_buffer);

    m_resBuf.push_back(*this);

    console.log(TypeLogs::LOG_INFO, XOR("Image with path {} loaded!"), path);
}

Resource::Resource(int resID, std::string_view type)
{
    HRSRC hResInfo = LF(FindResourceA)(globals::instance, MAKEINTRESOURCEA(resID), type.data());
    if (!hResInfo)
        throw std::runtime_error(XOR("Recource could not be found"));

    HGLOBAL hResData = LF(LoadResource)(globals::instance, hResInfo);
    if(!hResData)
        throw std::runtime_error(XOR("Recource data could not be found"));

    unsigned char* hResPtr = reinterpret_cast<unsigned char*>(LF(LockResource)(hResData));
    size_t size = LF(SizeofResource)(globals::instance, hResInfo);

    stbi_set_flip_vertically_on_load_thread(false);
    m_buffer = stbi_load_from_memory(hResPtr, size, &m_width, &m_height, nullptr, 4);
    // imgui
    m_texture = reinterpret_cast<IDirect3DTexture9*>(ImGui_CreateTexture(m_buffer, m_width, m_height));
    // surface
    surfaceRender.initNewTexture(m_textureID, m_buffer, m_width, m_height);
    stbi_image_free(m_buffer);
    LF(FreeResource)(hResData);

    m_resBuf.push_back(*this);

    console.log(TypeLogs::LOG_INFO, XOR("Resource ID {}:{} image loaded!"), resID, type);
}

Resource::~Resource()
{
    for (const auto& el : m_resBuf)
    {
        if (el.m_texture && el.m_textureID)
        {
            ImGui_DestroyTexture(m_texture);
            interfaces::surface->deleteTextureID(m_textureID);
        }
    }
}
