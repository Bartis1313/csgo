#pragma once

class AnimationLayer
{
public:
	float m_animationTime;
	float m_fadeOut;
	void* m_studio;
	int m_dispatchedSrc;
	int m_dispatchedDst;
	int m_order;
	size_t m_sequence;
	float m_prevCycle;
	float m_weight;
	float m_weightDeltaRate;
	float m_playbackRate;
	float m_cycle;
	void* m_entity;
	int m_invalidatePhysicsBits;
};