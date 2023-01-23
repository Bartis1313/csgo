#include "drawModel.hpp"

void DrawModelType::runAll(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix)
{
	for (auto el : m_hacksRun)
		el->run(result, state, info, matrix);
}