#pragma once

#include "math/Vector.hpp"
#include "CUserCmd.hpp"
#include "vars.hpp"

#include "helpers/pad.hpp"
#include "helpers/vfunc.hpp"

class Input
{
public:
    PAD(169);
    bool m_cameraInThirdPerson;
    PAD(2);
    Vec3 m_cameraOffset;
    PAD(56);
    CUserCmd* m_commands;
    CVerifiedUserCmd* m_verifiedCommands;

    VFUNC(CUserCmd*, getUserCmd, 8, (int slot, int sequence), (this, slot, sequence));

    CVerifiedUserCmd* getVerifiedUserCmd(int sequence)
    {
        return &m_verifiedCommands[sequence % MULTIPLAYER_BACKUP];
    }
};