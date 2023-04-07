#pragma once

#include "Recv.hpp"
#include "Enums.hpp"

class ClientClass
{
private:
	using createfn = std::add_pointer_t<void* __cdecl(int, int)>;
	using createeventfn = std::add_pointer_t<void* __cdecl()>;
public:
	createfn m_createFn;
	createeventfn m_createEventFn;
	char* m_networkName;
	RecvTable* m_recvTable;
	ClientClass* m_next;
	ClassID m_classID;
};