#pragma once

#include "math/Vector.hpp"

#include "helpers/pad.hpp"

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
	Vec3 m_origin;
	Vec3 m_angles;
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
	Vec3 m_shutterOpenPosition;
	Vec3 m_shutterOpenAngles;
	Vec3 m_shutterClosePosition;
	Vec3 m_shutterCloseAngles;
	float m_offCenterTop;
	float m_offCenterBottom;
	float m_offCenterLeft;
	float m_offCenterRight;
	int m_edgeBlur;
};