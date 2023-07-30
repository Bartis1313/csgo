#pragma once

struct MaterialData;
class IMatRenderContext;
struct DrawModelState_t;
struct ModelRenderInfo_t;

#include <SDK/math/matrix.hpp>

namespace materialEditor
{
	void draw();
	void initEditor();
	void passDME([[maybe_unused]]IMatRenderContext* ctx, [[maybe_unused]] const DrawModelState_t& state, [[maybe_unused]] const ModelRenderInfo_t& info, [[maybe_unused]] Matrix3x4* matrix);
}