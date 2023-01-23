#pragma once

#include "base.hpp"
#include <SDK/math/matrix.hpp>

#include <vector>

struct DrawModelState_t;
struct ModelRenderInfo_t;

class DrawModelType : protected BaseHack
{
public:
	constexpr DrawModelType() :
		BaseHack{}
	{
		m_hacksRun.push_back(this);
	}

public:
	static void runAll(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix);
protected:
	virtual void run(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix) = 0;
	// keep those methods to be nothing as default
	// mostly we don't need all of them
	virtual void init() override {};
	virtual void load() override {};
	virtual void reset() override {};
	virtual void shutdown() override {};
	virtual void roundRestart() override {};
private:
	inline static std::vector<DrawModelType*> m_hacksRun;
};