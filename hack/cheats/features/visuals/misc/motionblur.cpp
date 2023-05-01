#include "motionblur.hpp"

#include <SDK/CViewSetup.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <SDK/IMatRenderContext.hpp>
#include <SDK/ITexture.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/materialInit.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/math/math.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/console/console.hpp>
#include <gamememory/memory.hpp>

#include <cheats/hooks/overrideView.hpp>

namespace
{
	struct MotionHandler : hooks::OverrideView
	{
		MotionHandler()
		{
			this->registerRun(motionBlur::run);
		}
	} motionHandler;
}

namespace motionBlur
{
	struct MotionBlurHistory_t
	{
		float lastTimeUpdate{ };
		float previousPitch{ };
		float previousYaw{ };
		Vec3 previousPositon{ };
		float noRotationalMotionBlurUntil{ };
	} motionHistory;

	IMaterial* motion_blur;
	ITexture* _rt_FullFrameFB;

	std::array motionBlurValues{ 0.0f, 0.0f, 0.0f, 0.0f };
	std::array motionBlurViewportValues{ 0.0f, 0.0f, 0.0f, 0.0f };
}

void motionBlur::initMaterials()
{
	motion_blur = memory::interfaces::matSys->findMaterial("dev/motion_blur", TEXTURE_GROUP_RENDER_TARGET, false);
	_rt_FullFrameFB = memory::interfaces::matSys->findTexture("_rt_FullFrameFB", TEXTURE_GROUP_RENDER_TARGET);
}

// 1:1 from https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/client/viewpostprocess.cpp#L2996
// with small reduce of code because we dont care for portal/ps3 stuff and also we dont care for detecting blur, but knowing it's enabled
void motionBlur::run(CViewSetup* view)
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
		float timeElapsed = memory::interfaces::globalVars->m_realtime - motionHistory.lastTimeUpdate;

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
		Vec3 positionChange = motionHistory.previousPositon - currentPosition;

		if ((positionChange.length() > 30.0f) && (timeElapsed >= 0.5f))
		{
			resetArr(motionBlurValues, motionBlurValues.size());
		}
		else if (timeElapsed > (1.0f / 15.0f))
		{
			resetArr(motionBlurValues, motionBlurValues.size());
		}
		else if (positionChange.length() > 50.0f)
		{
			motionHistory.noRotationalMotionBlurUntil = memory::interfaces::globalVars->m_realtime + 1.0f;
		}
		else
		{
			const float horizontalFov = view->m_fov;
			const float verticalFov = (view->m_aspectRatio <= 0.0f) ? (view->m_fov) : (view->m_fov / view->m_aspectRatio);
			const float viewDotMotion = currentForwardVec.dot(positionChange);

			if (vars::misc->motionBlur->forward)
				motionBlurValues[2] = viewDotMotion;
			else
				motionBlurValues[2] = viewDotMotion * std::abs(currentForwardVec[2]);

			const float sideDotMotion = currentSideVec.dot(positionChange);
			float yawDiffOriginal = motionHistory.previousYaw - currentYaw;
			if (((motionHistory.previousYaw - currentYaw > 180.0f) || (motionHistory.previousYaw - currentYaw < -180.0f)) &&
				((motionHistory.previousYaw + currentYaw > -180.0f) && (motionHistory.previousYaw + currentYaw < 180.0f)))
				yawDiffOriginal = motionHistory.previousYaw + currentYaw;

			float yawDiffAdjusted = yawDiffOriginal + (sideDotMotion / 3.0f);

			if (yawDiffOriginal < 0.0f)
				yawDiffAdjusted = std::clamp(yawDiffAdjusted, yawDiffOriginal, 0.0f);
			else
				yawDiffAdjusted = std::clamp(yawDiffAdjusted, 0.0f, yawDiffOriginal);

			const float undampenedYaw = yawDiffAdjusted / horizontalFov;
			motionBlurValues[0] = undampenedYaw * (1.0f - (std::abs(currentPitch) / 90.0f));

			const float pitchCompensateMask = 1.0f - ((1.0f - std::abs(currentForwardVec[2])) * (1.0f - std::abs(currentForwardVec[2])));
			const float pitchDiffOriginal = motionHistory.previousPitch - currentPitch;
			float pitchdiffAdjusted = pitchDiffOriginal;

			if (currentPitch > 0.0f)
				pitchdiffAdjusted = pitchDiffOriginal - ((viewDotMotion / 2.0f) * pitchCompensateMask);
			else
				pitchdiffAdjusted = pitchDiffOriginal + ((viewDotMotion / 2.0f) * pitchCompensateMask);

			if (pitchDiffOriginal < 0.0f)
				pitchdiffAdjusted = std::clamp(pitchdiffAdjusted, pitchDiffOriginal, 0.0f);
			else
				pitchdiffAdjusted = std::clamp(pitchdiffAdjusted, 0.0f, pitchDiffOriginal);

			motionBlurValues[1] = pitchdiffAdjusted / verticalFov;
			motionBlurValues[3] = undampenedYaw;
			motionBlurValues[3] *= (std::abs(currentPitch) / 90.0f) * (std::abs(currentPitch) / 90.0f) * (std::abs(currentPitch) / 90.0f);

			if (timeElapsed > 0.0f)
				motionBlurValues[2] /= timeElapsed * 30.0f;
			else
				motionBlurValues[2] = 0.0f;

			motionBlurValues[2] = std::clamp((std::abs(motionBlurValues[2]) - vars::misc->motionBlur->fallingMin) / (vars::misc->motionBlur->fallingMax - vars::misc->motionBlur->fallingMin),
				0.0f, 1.0f) * (motionBlurValues[2] >= 0.0f ? 1.0f : -1.0f);
			motionBlurValues[2] /= 30.0f;
			motionBlurValues[0] *= vars::misc->motionBlur->rotationIntensity * vars::misc->motionBlur->strength;
			motionBlurValues[1] *= vars::misc->motionBlur->rotationIntensity * vars::misc->motionBlur->strength;
			motionBlurValues[2] *= vars::misc->motionBlur->fallingIntensity * vars::misc->motionBlur->strength;
			motionBlurValues[3] *= vars::misc->motionBlur->rollIntensity * vars::misc->motionBlur->strength;

			float slowFps = 30.0f;
			float fastFps = 50.0f;
			float currentFps = (timeElapsed > 0.0f) ? (1.0f / timeElapsed) : 0.0f;
			float dumpenFactor = std::clamp(((currentFps - slowFps) / (fastFps - slowFps)), 0.0f, 1.0f);

			motionBlurValues[0] *= dumpenFactor;
			motionBlurValues[1] *= dumpenFactor;
			motionBlurValues[2] *= dumpenFactor;
			motionBlurValues[3] *= dumpenFactor;
		}

		if (memory::interfaces::globalVars->m_realtime < motionHistory.noRotationalMotionBlurUntil)
		{
			resetArr(motionBlurValues, 3);
		}
		else
		{
			motionHistory.noRotationalMotionBlurUntil = 0.0f;
		}

		motionHistory.previousPositon = currentPosition;
		motionHistory.previousPitch = currentPitch;
		motionHistory.previousYaw = currentYaw;
		motionHistory.lastTimeUpdate = memory::interfaces::globalVars->m_realtime;

		int x = view->x;
		int y = view->y;
		int w = view->m_width;
		int h = view->m_height;

		float srcWidth = static_cast<float>(_rt_FullFrameFB->getActualWidth());
		float srcHeight = static_cast<float>(_rt_FullFrameFB->getActualHeight());
		int offset;
		offset = (x > 0) ? 1 : 0;
		motionBlurViewportValues[0] = static_cast<float>(x + offset) / (srcWidth - 1);

		offset = (x < (srcWidth - 1)) ? -1 : 0;
		motionBlurViewportValues[3] = static_cast<float>(x + w + offset) / (srcWidth - 1);

		offset = (y > 0) ? 1 : 0;
		motionBlurViewportValues[1] = static_cast<float>(y + offset) / (srcHeight - 1);

		offset = (y < (srcHeight - 1)) ? -1 : 0;
		motionBlurViewportValues[2] = static_cast<float>(y + h + offset) / (srcHeight - 1);

		for (auto& el : motionBlurViewportValues)
		{
			if (el <= 0.0f)
				el = -1.0f;
			else if (el >= 1.0f)
				el = 2.0f;
		}
	}
}

void motionBlur::drawBlur()
{
	IMaterialVar* MotionBlurInternal = motion_blur->findVar("$MotionBlurInternal", nullptr, false);

	MotionBlurInternal->setVectorComponent(motionBlurValues[0], 0);
	MotionBlurInternal->setVectorComponent(motionBlurValues[1], 1);
	MotionBlurInternal->setVectorComponent(motionBlurValues[2], 2);
	MotionBlurInternal->setVectorComponent(motionBlurValues[3], 3);

	// todo: find easiest way to trick the game we run motion blur, (easiest and best way - mempatch correct bytes)

	// edit those fields from memory, todo
	//float* motionBlurIntervalValues = *reinterpret_cast<float**>(m_motionBlurAddr);
	//motionBlurIntervalValues = m_motionBlurValues.data();

	IMaterialVar* MotionBlurViewPortInternal = motion_blur->findVar("$MotionBlurViewportInternal", nullptr, false);

	MotionBlurViewPortInternal->setVectorComponent(motionBlurViewportValues[0], 0);
	MotionBlurViewPortInternal->setVectorComponent(motionBlurViewportValues[1], 1);
	MotionBlurViewPortInternal->setVectorComponent(motionBlurViewportValues[2], 2);
	MotionBlurViewPortInternal->setVectorComponent(motionBlurViewportValues[3], 3);

	auto ctx = memory::interfaces::matSys->getRenderContext();
	ctx->drawScreenEffectMaterial(motion_blur);
}

void motionBlur::render()
{
	INIT_MATERIALS_ONCE(initMaterials);

	if (!vars::misc->motionBlur->enabled)
		return;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	drawBlur();
}
