#include "tracerDraw.hpp"

hooks::TracerDraw::value hooks::TracerDraw::hook(void* mesh, void* start, void* end, float w, float* color)
{
	printf("working\n");
	original(mesh, start, end, w, color);
}