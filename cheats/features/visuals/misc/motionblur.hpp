#pragma once

#include <classes/overrideView.hpp>
#include <SDK/math/Vector.hpp>

#include <array>

class CViewSetup;
struct Vector;

class MotionBlur : public OverrideViewType
{
public:
	MotionBlur() :
		OverrideViewType{}
	{}
	
	virtual void init();
	virtual void run(CViewSetup* view);
	// no point in making it virtual
	void render();
private:
	struct MotionBlurHistory_t
	{
		MotionBlurHistory_t() :
			m_lastTimeUpdate{ 0.0f }, m_previousPitch{ 0.0f }, m_previousYaw{ 0.0f },
			m_previousPositon{ Vector{} }, m_noRotationalMotionBlurUntil{ 0.0f }
		{}

		float m_lastTimeUpdate;
		float m_previousPitch;
		float m_previousYaw;
		Vector m_previousPositon;
		float m_noRotationalMotionBlurUntil;
	} m_motionHistory;

	std::array<float, 4> m_motionBlurValues = { 0.0f, 0.0f, 0.0f, 0.0f };
	std::array<float, 4> m_motionBlurViewportValues = { 0.0f, 0.0f, 0.0f, 0.0f };
};

[[maybe_unused]] inline auto g_MotionBlur = MotionBlur{};
