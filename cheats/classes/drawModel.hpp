#pragma once

#include "base.hpp"

#include <vector>

struct DrawModelState_t;
struct ModelRenderInfo_t;
struct Matrix3x4;

class DrawModelType : public BaseHack
{
public:
	constexpr DrawModelType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

	virtual void run(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix) {};
	static void runAll(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix);
protected:
	inline static std::vector<DrawModelType*> m_hacksRun;
};