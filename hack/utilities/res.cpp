#include "res.hpp"

#include "tools/tools.hpp"
#include <render/render.hpp>
#include "../utilities/console/console.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <imgui_impl_dx9.h>
#include <cheats/game/globals.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/ISurface.hpp>
#include <menu/GUI-ImGui/imguiaddons.hpp>

#include <Windows.h>
#include <d3dx9.h>

// not going to use D3DXCreateTextureFromFileA or D3DXCreateTextureFromResourceA
// first - this API is very old and second D3DXCreateTextureFromResourceA had problems and my resource was forced to be RCDATA type instead of generated one to work.

Resource::Resource(const std::string& path)
{
	if (path.empty())
		return;

	stbi_set_flip_vertically_on_load_thread(false);
	m_buffer = stbi_load(path.c_str(), &m_width, &m_height, nullptr, 4);
	if (!m_buffer)
	{
		console::error("Creating resource from path failed, reason: {}", stbi_failure_reason());
		return;
	}
	m_texture = reinterpret_cast<IDirect3DTexture9*>(ImGui_CreateTexture(m_buffer, m_width, m_height));
	surfaceRender.initNewTexture(m_textureID, m_buffer, m_width, m_height);
	stbi_image_free(m_buffer);

	m_resBuf.push_back(*this);

	console::debug("Image with path {} loaded!", path);
}

Resource::Resource(int resID, const std::string_view type)
{
	HRSRC hResInfo = LI_FN_CACHED(FindResourceA)(globals::instance, MAKEINTRESOURCEA(resID), type.data());
	if (!hResInfo)
		throw std::runtime_error("Recource could not be found");

	HGLOBAL hResData = LI_FN_CACHED(LoadResource)(globals::instance, hResInfo);
	if(!hResData)
		throw std::runtime_error("Recource data could not be found");

	unsigned char* hResPtr = reinterpret_cast<unsigned char*>(LI_FN_CACHED(LockResource)(hResData));
	size_t size = LI_FN_CACHED(SizeofResource)(globals::instance, hResInfo);

	stbi_set_flip_vertically_on_load_thread(false);
	m_buffer = stbi_load_from_memory(hResPtr, size, &m_width, &m_height, nullptr, 4);
	if (!m_buffer)
	{
		console::error("Creating resource from resources file failed, reason: {}", stbi_failure_reason());
		return;
	}
	m_texture = reinterpret_cast<IDirect3DTexture9*>(ImGui_CreateTexture(m_buffer, m_width, m_height));
	surfaceRender.initNewTexture(m_textureID, m_buffer, m_width, m_height);
	stbi_image_free(m_buffer);
	LI_FN_CACHED(FreeResource)(hResData);

	m_resBuf.push_back(*this);

	console::debug("Resource ID {}:{} image loaded!", resID, type);
}

Resource::Resource(void* data, size_t size)
{
	stbi_set_flip_vertically_on_load_thread(false);
	m_buffer = stbi_load_from_memory(reinterpret_cast<unsigned char*>(data), size, &m_width, &m_height, nullptr, 4);
	if (!m_buffer)
	{
		console::error("Creating resource from memory failed, reason: {}", stbi_failure_reason());
		return;
	}
	m_texture = reinterpret_cast<IDirect3DTexture9*>(ImGui_CreateTexture(m_buffer, m_width, m_height));
	stbi_image_free(m_buffer);

	m_resBuf.push_back(*this);

	console::debug("Image from memory loaded!");
}

void Resource::destroyAll()
{
	for (const auto& el : m_resBuf)
	{
		if (el.getTexture() && el.getTextureID())
		{
			ImGui_DestroyTexture(el.getTexture());
			memory::interfaces::surface->deleteTextureID(el.getTextureID());
		}
	}
}
