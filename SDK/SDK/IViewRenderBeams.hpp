#pragma once

#include "IEngineTrace.hpp"
#include "IClientEntityList.hpp"
#include "Enums.hpp"
#include "math/Vector.hpp"

class Entity_t;

struct BeamInfo_t
{
	int m_type;
	Entity_t* m_startEnt;
	int m_startAttachment;
	Entity_t* m_endEnt;
	int m_EndAttachment;
	Vector m_vecStart;
	Vector m_vecEnd;
	int m_modelIndex;
	const char* m_modelName;
	int m_haloIndex;
	const char* m_haloName;
	float m_haloScale;
	float m_life;
	float m_width;
	float m_endWidth;
	float m_fadeLength;
	float m_amplitude;
	float m_brightness;
	float m_speed;
	int m_startFrame;
	float m_frameRate;
	float m_red;
	float m_green;
	float m_blue;
	bool m_renderable;
	int m_segments;
	int m_flags;
	Vector m_vecCenter;
	float m_startRadius;
	float m_endRadius;

	BeamInfo_t()
	{
		m_type = TE_BEAMPOINTS;
		m_segments = -1;
		m_modelName = nullptr;
		m_haloName = nullptr;
		m_modelIndex = -1;
		m_haloIndex = -1;
		m_renderable = true;
		m_flags = 0;
	}
};

class Beam_t;

class IViewRenderBeams
{
public:
	VFUNC(void, drawBeam, 4, (Beam_t* beam), (this, beam));
	VFUNC(Beam_t*, createBeamPoints, 12, (BeamInfo_t& beamInfo), (this, std::ref(beamInfo)));
	VFUNC(Beam_t*, createRingPoint, 16, (BeamInfo_t& beamInfo), (this, std::ref(beamInfo)));
};