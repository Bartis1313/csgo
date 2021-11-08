#pragma once
#include "../utilities/vfunc.hpp"

class IBaseClientDLL
{
public:
    VFUNC(ClientClass*, getAllClasses, 8, (), (this));
    VFUNC(bool, dispatchUserMsg, 38, (int msgType, unsigned int arg1, unsigned int length, const void* data = nullptr), (this, msgType, arg1, length, data));
};