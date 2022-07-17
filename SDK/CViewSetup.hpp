#pragma once

#include "math/Vector.hpp"
#include "../utilities/pad.hpp"

class CViewSetup
{
public:
	int x;
	int xOld;
	int y;
	int yOld;
	int m_width;
	int m_widthOld;
	int m_height;
	int m_heightOld;
	PAD(144);
	float m_fov;
	float m_viewModelFov;
	Vector m_origin;
	Vector m_angles;
	float m_nearZ;
	float m_farZ;
	float m_nearViewModelZ;
	float m_farViewModelZ;
	float m_aspectRatio;
	float m_nearBlurDepth;
	float m_nearFocusDepth;
	float m_farFocusDepth;
	float m_farBlurDepth;
	float m_nearBlurRadius;
	float m_farBlurRadius;
	float m_doFQuality;
	int m_motionBlurMode;
	float m_shutterTime;
	Vector m_shutterOpenPosition;
	Vector m_shutterOpenAngles;
	Vector m_shutterClosePosition;
	Vector m_shutterCloseAngles;
	float m_offCenterTop;
	float m_offCenterBottom;
	float m_offCenterLeft;
	float m_offCenterRight;
	bool m_offCenter : 1;
	bool m_renderToSubrectOfLargerScreen : 1;
	bool m_doBloomAndToneMapping : 1;
	bool m_doDepthOfField : 1;
	bool m_HDRTarget : 1;
	bool m_drawWorldNormal : 1;
	bool m_cullFontFaces : 1;
	bool m_cacheFullSceneState : 1;
	bool m_CSMView : 1;
};