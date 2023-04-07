#include "mirrorCam.hpp"

#include <d3d9.h>

#include <imgui.h>

#include <SDK/interfaces/interfaces.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <SDK/ITexture.hpp>
#include <SDK/IMatRenderContext.hpp>
#include <SDK/CViewSetup.hpp>
#include <SDK/IVRenderView.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/materialInit.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/inputSystem.hpp>
#include <utilities/console/console.hpp>
#include <cheats/hooks/hooks.hpp>
#include <cheats/game/game.hpp>
#include <config/vars.hpp>

void MirrorCam::initTexture()
{
	m_texture = memory::interfaces::matSys->createFullFrameRenderTarget("mirrorCam");
}

void MirrorCam::updateKeys()
{
	vars::keys->mirrorCam.update();
}

void MirrorCam::run(const CViewSetup& view)
{
	INIT_MATERIALS_ONCE(initTexture);

	// this might eat some FPS, if enabled
	if (!vars::misc->mirrorCam->enabled)
		return;

	CViewSetup v = view;

	v.m_angles[Coord::Y] = v.m_angles[Coord::Y] + 180.0f; // back
	v.x = v.xOld = 0;
	v.y = v.yOld = 0;
	v.m_width = v.m_widthOld = static_cast<int>(m_size[Coord::X]);
	v.m_height = v.m_heightOld = static_cast<int>(m_size[Coord::Y]);
	v.m_aspectRatio = static_cast<float>(v.m_width / v.m_height);
	v.m_nearZ = v.m_nearViewModelZ = 7.0f;
	v.m_fov = 50.0f;

	auto ctx = memory::interfaces::matSys->getRenderContext();
	ctx->pushRenderTargetAndViewport();
	ctx->setRenderTarget(m_texture);

	hooks::viewRender::original(memory::interfaces::viewRender(), v, v, VIEW_CLEAR_COLOR | VIEW_CLEAR_DEPTH | VIEW_CLEAR_STENCIL, 0);

	ctx->popRenderTargetAndViewport();
	ctx->release();
}

IDirect3DTexture9* MirrorCam::getTexture() const
{
	return m_texture->m_handle[0]->m_texture;
}

void MirrorCamDraw::draw()
{
	if (!game::isAvailable())
		return;

	if (!vars::misc->mirrorCam->enabled)
		return;

	if (vars::misc->mirrorCam->onKey)
	{
		if (!vars::keys->mirrorCam.isEnabled())
			return;
	}

	if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/tier2/renderutils.cpp#L988
		// ^ is proper scaling what game does

		if (auto size = ImGui::GetContentRegionAvail(); size.x != 0.0f && size.y != 0.0f) // / 0
		{
			float xRatio = static_cast<float>(g_MirrorCam->m_texture->getActualWidth()) / size.x;
			float yRatio = static_cast<float>(g_MirrorCam->m_texture->getActualHeight()) / size.y;

			g_MirrorCam->setSize(Vec2{ size.x * xRatio, size.y * yRatio });
			ImGui::Image(g_MirrorCam->getTexture(), size);
		}

		ImGui::End();
	}
}
