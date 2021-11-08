#pragma once
#include "math/Vector.hpp"
#include "../utilities/pad.hpp"

class CUserCmd
{
public:
    PAD(4);
    int m_commandNumber;
    int m_tickcount;
    Vector m_viewangles;
    Vector m_aimdirection;
    float m_forwardmove;
    float m_sidemove;
    float m_upmove;
    int m_buttons;
    char m_impulse;
    int m_weaponSelect;
    int m_weaponSubtype;
    int m_randomSeed;
    short m_mousedx;
    short m_mousedy;
    bool m_predicted;
};

class CVerifiedUserCmd
{
public:
    CUserCmd m_cmd;
    unsigned long m_crc;
};