#pragma once

#include <cstdint>

class CFlashlightEffect
{
public:
	bool m_isOn;
private:
	char someweirdbuf[3]; // could be bools
public:
	int m_entIndex;
	uint16_t m_flashlightHandle;
	bool m_bigMode; // dunno how to name it
	bool m_muzzleFlashEnabled;
	float m_muzzleFlashBrightness;
	float m_fov;
	float m_farZ;
	float m_LinearAtten;
	bool m_castsShadows;
private:
	char someweirdbuf2[3]; // could be bools
public:
	float m_currentPullBackDist;
	uint32_t m_muzzleFlashTexture;
	uint32_t m_flashLightTexture;
	char m_textureName[64];
};