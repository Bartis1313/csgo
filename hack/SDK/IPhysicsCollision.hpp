#pragma once

#include "helpers/vfunc.hpp"
#include "IVModelInfo.hpp"

class IPhysicsCollision
{
public:
    VFUNC(void, vcollideLoad, 36, (vcollide_t* output, int solidCount, const char* buffer, int size, bool swap = false), (this, output, solidCount, buffer, size, swap));
    VFUNC(void, vcollideUnload, 37, (vcollide_t* collide), (this, collide));
};