#pragma once
#include "Recv.hpp"
#include "math/Vector.hpp"
#include "../utilities/pad.hpp"

class CViewSetup
{
public:
	int x;
	int xOld;
	int y;
	int yOld;
	int m_width;
	int m_widthOld;
	int m_height;
	int m_heightOld;
	PAD(144);
	float m_fov;
	float m_viewmodelFov;
	Vector m_origin;
	Vector m_angles;
	PAD(124);
};

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