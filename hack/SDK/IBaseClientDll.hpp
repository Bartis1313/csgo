#pragma once

#include "helpers/vfunc.hpp"

class ClientClass;

class IBaseClientDLL
{
public:
	VFUNC(ClientClass*, getAllClasses, 8, (), (this));
	VFUNC(bool, dispatchUserMsg, 38, (int msgType, size_t arg1, size_t length, const void* data = nullptr), (this, msgType, arg1, length, data));
};