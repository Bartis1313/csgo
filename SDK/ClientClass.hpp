#pragma once
#include "Recv.hpp"

class ClientClass
{
public:
	void* m_createFn;
	void* m_createEventFn;
	char* m_networkName;
	RecvTable* m_recvTable;
	ClientClass* m_next;
	int m_classID;
};