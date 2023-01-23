#pragma once

#include "helpers/netvars.hpp"
#include "helpers/vfunc.hpp"
#include "math/Vector.hpp"

// Each mod defines these for itself.
class CViewVectors
{
public:
	CViewVectors() {}

	CViewVectors(
		Vec3 vView,
		Vec3 vHullMin,
		Vec3 vHullMax,
		Vec3 vDuckHullMin,
		Vec3 vDuckHullMax,
		Vec3 vDuckView,
		Vec3 vObsHullMin,
		Vec3 vObsHullMax,
		Vec3 vDeadViewHeight)
	{
		m_vView = vView;
		m_vHullMin = vHullMin;
		m_vHullMax = vHullMax;
		m_vDuckHullMin = vDuckHullMin;
		m_vDuckHullMax = vDuckHullMax;
		m_vDuckView = vDuckView;
		m_vObsHullMin = vObsHullMin;
		m_vObsHullMax = vObsHullMax;
		m_vDeadViewHeight = vDeadViewHeight;
	}

	// Height above entity position where the viewer's eye is.
	Vec3 m_vView;

	Vec3 m_vHullMin;
	Vec3 m_vHullMax;

	Vec3 m_vDuckHullMin;
	Vec3 m_vDuckHullMax;
	Vec3 m_vDuckView;

	Vec3 m_vObsHullMin;
	Vec3 m_vObsHullMax;

	Vec3 m_vDeadViewHeight;
};

class CGameRules
{
public:
	NETVAR(int, m_iRoundTime, "DT_CSGameRulesProxy", "m_iRoundTime");

	VFUNC(CViewVectors*, getViewVectors, 30, (), (this));
};