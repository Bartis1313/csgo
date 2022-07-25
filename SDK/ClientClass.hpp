#pragma once
#include "Recv.hpp"

class ClientClass
{
private:
	using createfn = void* (__cdecl*)(int, int);
	using createeventfn = void* (__cdecl*)();
public:
	createfn m_createFn;
	createeventfn m_createEventFn;
	char* m_networkName;
	RecvTable* m_recvTable;
	ClientClass* m_next;
	int m_classID;
};