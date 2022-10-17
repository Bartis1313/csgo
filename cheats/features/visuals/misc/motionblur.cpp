#include "motionblur.hpp"

#include <SDK/CViewSetup.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <SDK/ITexture.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/math/math.hpp>
#include <utilities/tools/tools.hpp>
#include <gamememory/memory.hpp>

// 1:1 from https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/client/viewpostprocess.cpp#L2996
// with small reduce of code because we dont care for portal/ps3 stuff and also we dont care for detecting blur, but knowing it's enabled
void MotionBlur::run(CViewSetup* view)
{
	if (!vars::misc->motionBlur->enabled)
		return;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	constexpr auto resetArr = [](std::array<float, 4>& arr, size_t indexesToReset)
	{
		for (size_t i = 0; auto & el : arr)
		{
			if (i == indexesToReset)
				break;

			el = 0.0f;

			i++;
		}
	};

	if (view)
	{
		float timeElapsed = interfaces::globalVars->m_realtime - m_motionHistory.m_lastTimeUpdate;

		float currentPitch = view->m_angles[Coord::X];
		while (currentPitch > 180.0f)
			currentPitch -= 360.0f;
		while (currentPitch < -180.0f)
			currentPitch += 360.0f;

		float currentYaw = view->m_angles[Coord::Y];
		while (currentYaw > 180.0f)
			currentYaw -= 360.0f;
		while (currentYaw < -180.0f)
			currentYaw += 360.0f;

		auto [currentForwardVec, currentSideVec, uselessVec ] = math::angleVectors(view->m_angles);

		Vec3 currentPosition = view->m_origin;
		Vec3 positionChange = m_motionHistory.m_previousPositon - currentPosition;

		if ((positionChange.length() > 30.0f) && (timeElapsed >= 0.5f))
		{
			resetArr(m_motionBlurValues, m_motionBlurValues.size());
		}
		else if (timeElapsed > (1.0f / 15.0f))
		{
			resetArr(m_motionBlurValues, m_motionBlurValues.size());
		}
		else if (positionChange.length() > 50.0f)
		{
			m_motionHistory.m_noRotationalMotionBlurUntil = interfaces::globalVars->m_realtime + 1.0f;
		}
		else
		{
			const float horizontalFov = view->m_fov;
			const float verticalFov = (view->m_aspectRatio <= 0.0f) ? (view->m_fov) : (view->m_fov / view->m_aspectRatio);
			const float viewDotMotion = currentForwardVec.dot(positionChange);

			if (vars::misc->motionBlur->forward)
				m_motionBlurValues[2] = viewDotMotion;
			else
				m_motionBlurValues[2] = viewDotMotion * std::abs(currentForwardVec[2]);

			const float sideDotMotion = currentSideVec.dot(positionChange);
			float yawDiffOriginal = m_motionHistory.m_previousYaw - currentYaw;
			if (((m_motionHistory.m_previousYaw - currentYaw > 180.0f) || (m_motionHistory.m_previousYaw - currentYaw < -180.0f)) &&
				((m_motionHistory.m_previousYaw + currentYaw > -180.0f) && (m_motionHistory.m_previousYaw + currentYaw < 180.0f)))
				yawDiffOriginal = m_motionHistory.m_previousYaw + currentYaw;

			float yawDiffAdjusted = yawDiffOriginal + (sideDotMotion / 3.0f);

			if (yawDiffOriginal < 0.0f)
				yawDiffAdjusted = std::clamp(yawDiffAdjusted, yawDiffOriginal, 0.0f);
			else
				yawDiffAdjusted = std::clamp(yawDiffAdjusted, 0.0f, yawDiffOriginal);

			const float undampenedYaw = yawDiffAdjusted / horizontalFov;
			m_motionBlurValues[0] = undampenedYaw * (1.0f - (std::abs(currentPitch) / 90.0f));

			const float pitchCompensateMask = 1.0f - ((1.0f - std::abs(currentForwardVec[2])) * (1.0f - std::abs(currentForwardVec[2])));
			const float pitchDiffOriginal = m_motionHistory.m_previousPitch - currentPitch;
			float pitchdiffAdjusted = pitchDiffOriginal;

			if (currentPitch > 0.0f)
				pitchdiffAdjusted = pitchDiffOriginal - ((viewDotMotion / 2.0f) * pitchCompensateMask);
			else
				pitchdiffAdjusted = pitchDiffOriginal + ((viewDotMotion / 2.0f) * pitchCompensateMask);

			if (pitchDiffOriginal < 0.0f)
				pitchdiffAdjusted = std::clamp(pitchdiffAdjusted, pitchDiffOriginal, 0.0f);
			else
				pitchdiffAdjusted = std::clamp(pitchdiffAdjusted, 0.0f, pitchDiffOriginal);

			m_motionBlurValues[1] = pitchdiffAdjusted / verticalFov;
			m_motionBlurValues[3] = undampenedYaw;
			m_motionBlurValues[3] *= (std::abs(currentPitch) / 90.0f) * (std::abs(currentPitch) / 90.0f) * (std::abs(currentPitch) / 90.0f);

			if (timeElapsed > 0.0f)
				m_motionBlurValues[2] /= timeElapsed * 30.0f;
			else
				m_motionBlurValues[2] = 0.0f;

			m_motionBlurValues[2] = std::clamp((std::abs(m_motionBlurValues[2]) - vars::misc->motionBlur->fallingMin) / (vars::misc->motionBlur->fallingMax - vars::misc->motionBlur->fallingMin), 0.0f, 1.0f) * (m_motionBlurValues[2] >= 0.0f ? 1.0f : -1.0f);
			m_motionBlurValues[2] /= 30.0f;
			m_motionBlurValues[0] *= vars::misc->motionBlur->rotationIntensity * vars::misc->motionBlur->strength;
			m_motionBlurValues[1] *= vars::misc->motionBlur->rotationIntensity * vars::misc->motionBlur->strength;
			m_motionBlurValues[2] *= vars::misc->motionBlur->fallingIntensity * vars::misc->motionBlur->strength;
			m_motionBlurValues[3] *= vars::misc->motionBlur->rollIntensity * vars::misc->motionBlur->strength;

			float slowFps = 30.0f;
			float fastFps = 50.0f;
			float currentFps = (timeElapsed > 0.0f) ? (1.0f / timeElapsed) : 0.0f;
			float dumpenFactor = std::clamp(((currentFps - slowFps) / (fastFps - slowFps)), 0.0f, 1.0f);

			m_motionBlurValues[0] *= dumpenFactor;
			m_motionBlurValues[1] *= dumpenFactor;
			m_motionBlurValues[2] *= dumpenFactor;
			m_motionBlurValues[3] *= dumpenFactor;
		}

		if (interfaces::globalVars->m_realtime < m_motionHistory.m_noRotationalMotionBlurUntil)
		{
			resetArr(m_motionBlurValues, 3);
		}
		else
		{
			m_motionHistory.m_noRotationalMotionBlurUntil = 0.0f;
		}

		m_motionHistory.m_previousPositon = currentPosition;
		m_motionHistory.m_previousPitch = currentPitch;
		m_motionHistory.m_previousYaw = currentYaw;
		m_motionHistory.m_lastTimeUpdate = interfaces::globalVars->m_realtime;

		static ITexture* _rt_FullFrameFB = interfaces::matSys->findTexture("_rt_FullFrameFB", TEXTURE_GROUP_RENDER_TARGET);

		int x = view->x;
		int y = view->y;
		int w = view->m_width;
		int h = view->m_height;

		float srcWidth = static_cast<float>(_rt_FullFrameFB->getActualWidth());
		float srcHeight = static_cast<float>(_rt_FullFrameFB->getActualHeight());
		int offset;
		offset = (x > 0) ? 1 : 0;
		m_motionBlurViewportValues[0] = static_cast<float>(x + offset) / (srcWidth - 1);

		offset = (x < (srcWidth - 1)) ? -1 : 0;
		m_motionBlurViewportValues[3] = static_cast<float>(x + w + offset) / (srcWidth - 1);

		offset = (y > 0) ? 1 : 0;
		m_motionBlurViewportValues[1] = static_cast<float>(y + offset) / (srcHeight - 1);

		offset = (y < (srcHeight - 1)) ? -1 : 0;
		m_motionBlurViewportValues[2] = static_cast<float>(y + h + offset) / (srcHeight - 1);

		for (auto& el : m_motionBlurViewportValues)
		{
			if (el <= 0.0f)
				el = -1.0f;
			else if (el >= 1.0f)
				el = 2.0f;
		}
	}
}

void MotionBlur::render()
{
	if (!vars::misc->motionBlur->enabled)
		return;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	static IMaterial* motion_blur = interfaces::matSys->findMaterial(XOR("dev/motion_blur"), XOR(TEXTURE_GROUP_RENDER_TARGET), false);

	static IMaterialVar* MotionBlurInternal = motion_blur->findVar(XOR("$MotionBlurInternal"), nullptr, false);

	MotionBlurInternal->setVectorComponent(m_motionBlurValues[0], 0);
	MotionBlurInternal->setVectorComponent(m_motionBlurValues[1], 1);
	MotionBlurInternal->setVectorComponent(m_motionBlurValues[2], 2);
	MotionBlurInternal->setVectorComponent(m_motionBlurValues[3], 3);
	
	// todo: find easiest way to trick the game we run motion blur, (easiest and best way - mempatch correct bytes)

	// edit those fields from memory, todo
	//float* motionBlurIntervalValues = *reinterpret_cast<float**>(m_motionBlurAddr);
	//motionBlurIntervalValues = m_motionBlurValues.data();

	static IMaterialVar* MotionBlurViewPortInternal = motion_blur->findVar(XOR("$MotionBlurViewportInternal"), nullptr, false);

	MotionBlurViewPortInternal->setVectorComponent(m_motionBlurViewportValues[0], 0);
	MotionBlurViewPortInternal->setVectorComponent(m_motionBlurViewportValues[1], 1);
	MotionBlurViewPortInternal->setVectorComponent(m_motionBlurViewportValues[2], 2);
	MotionBlurViewPortInternal->setVectorComponent(m_motionBlurViewportValues[3], 3);

	// this code will make fullscreen blured, you probably don't want it. Also group will fuck up everything
	// TEXTURE_GROUP_CLIENT_EFFECTS will pass
	/*auto ctx = interfaces::matSys->getRenderContext();
	ctx->drawScreenSpaceRectangle(motion_blur, 0, 0, globals::screenX, globals::screenY, 0, 0,
		globals::screenX, globals::screenY, globals::screenX, globals::screenY);
	ctx->release();*/

	// so we have to wrap it and call directly in isdepth hook to prevent problems like weapon being blurred
	const static auto _call = g_Memory.m_drawSpacedRectangle();
	__asm
	{
		push globals::screenY
		push globals::screenX
		push 0
		xor edx, edx
		mov ecx, motion_blur
		call _call
		add esp, 12
	}
}
