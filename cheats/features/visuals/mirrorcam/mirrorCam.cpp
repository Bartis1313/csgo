#include "mirrorCam.hpp"

#include <d3d9.h>

#include <dependencies/ImGui/imgui.h>

#include <SDK/interfaces/interfaces.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <SDK/ITexture.hpp>
#include <SDK/IMatRenderContext.hpp>
#include <SDK/CViewSetup.hpp>
#include <SDK/IVRenderView.hpp>
#include <SDK/CGlobalVars.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/inputSystem.hpp>
#include <utilities/console/console.hpp>
#include <hooks/hooks.hpp>
#include <game/game.hpp>
#include <config/vars.hpp>

void MirrorCam::init()
{
	//interfaces::matSys->forceBeginRenderTargetAllocation();
	m_texture = interfaces::matSys->createFullFrameRenderTarget(XOR("mirrorCam"));
	//interfaces::matSys->forceEndRenderTargetAllocation();

	m_inited = true;

	console.log(TypeLogs::LOG_INFO, XOR("Inited mirrorcam texture!"));
}

void MirrorCam::run(const CViewSetup& view)
{
	// this might eat some FPS, if enabled
	if (!config.get<bool>(vars.bMirrorCam))
		return;

	CViewSetup v = std::move(view);

	v.m_angles.y = v.m_angles.y + 180.0f; // back
	v.x = v.xOld = 0;
	v.y = v.yOld = 0;
	v.m_width = v.m_widthOld = static_cast<int>(m_size.x);
	v.m_height = v.m_heightOld = static_cast<int>(m_size.y);
	v.m_aspectRatio = static_cast<float>(v.m_width / v.m_height);
	v.m_nearZ = v.m_nearViewModelZ = 7.0f;
	v.m_fov = 50.0f;

	auto ctx = interfaces::matSys->getRenderContext();
	ctx->pushRenderTargetAndViewport();
	ctx->setRenderTarget(m_texture);

	hooks::renderView::original(interfaces::viewRender, v, v, VIEW_CLEAR_COLOR | VIEW_CLEAR_DEPTH | VIEW_CLEAR_STENCIL, 0);

	ctx->popRenderTargetAndViewport();
	ctx->release();
}

IDirect3DTexture9* MirrorCam::getTexture() const
{
	return m_texture->m_handle[0]->m_texture;
}

void MirrorCamDraw::init()
{

}

void MirrorCamDraw::draw()
{
	if (!g_MirrorCam.m_inited)
		return;

	if (!game::isAvailable())
		return;

	if (!config.get<bool>(vars.bMirrorCam))
		return;

	if (config.get<bool>(vars.bMirrorCamOnKey))
	{
		if (!config.get<Key>(vars.kMirrorCam).isEnabled())
			return;
	}

	// useless, use getActual...
	// raw sizes, returns ur screen size
	/*D3DSURFACE_DESC surfaceDesc;
	m_texture->m_handle[0]->m_texture->GetLevelDesc(0, &surfaceDesc);*/

	if (ImGui::Begin(XOR("Camera"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/tier2/renderutils.cpp#L988
		// ^ is proper scaling what game does

		if (auto size = ImGui::GetContentRegionAvail(); size.x != 0.0f && size.y != 0.0f) // / 0
		{
			float xRatio = static_cast<float>(g_MirrorCam.m_texture->getActualWidth()) / size.x;
			float yRatio = static_cast<float>(g_MirrorCam.m_texture->getActualHeight()) / size.y;

			g_MirrorCam.setSize({ size.x * xRatio, size.y * yRatio });
			ImGui::Image(g_MirrorCam.getTexture(), size);
		}

		ImGui::End();
	}
}
