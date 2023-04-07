#pragma once

#include <cheats/classes/overrideView.hpp>
#include <SDK/math/Vector.hpp>

#include <array>

class CViewSetup;
class IMaterial;
class ITexture;

class MotionBlur : protected OverrideViewType
{
public:
	MotionBlur() :
		OverrideViewType{}
	{}

	void render();
protected:
	virtual void run(CViewSetup* view) override;
private:
	void initMaterials();
	void drawBlur();

	struct MotionBlurHistory_t
	{
		constexpr MotionBlurHistory_t() :
			m_lastTimeUpdate{ 0.0f }, m_previousPitch{ 0.0f }, m_previousYaw{ 0.0f },
			m_previousPositon{ Vec3{} }, m_noRotationalMotionBlurUntil{ 0.0f }
		{}

		float m_lastTimeUpdate{ };
		float m_previousPitch{ };
		float m_previousYaw{ };
		Vec3 m_previousPositon{ };
		float m_noRotationalMotionBlurUntil{ };
	} m_motionHistory{ };

	IMaterial* motion_blur;
	ITexture* _rt_FullFrameFB;

	std::array<float, 4> m_motionBlurValues{ 0.0f, 0.0f, 0.0f, 0.0f };
	std::array<float, 4> m_motionBlurViewportValues{ 0.0f, 0.0f, 0.0f, 0.0f };
};

GLOBAL_FEATURE(MotionBlur);
