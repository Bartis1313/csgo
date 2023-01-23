#pragma once

#include "helpers/vfunc.hpp"

class IGameEvent
{
public:
    VFUNC(const char*, getName, 1, (), (this));
    VFUNC(int, getInt, 6, (const char* keyName, int defaultValue = 0), (this, keyName, defaultValue));
    VFUNC(float, getFloat, 8, (const char* keyName, float defaultValue = 0.0f), (this, keyName, defaultValue));
    VFUNC(const char*, getString, 9, (const char* keyName, const char* defaultValue = ""), (this, keyName, defaultValue));
    VFUNC(void, setInt, 13, (const char* keyName, int value), (this, keyName, value));
    VFUNC(void, setString, 16, (const char* keyName, const char* value), (this, keyName, value));
};

class IGameEventListener
{
public:
    virtual ~IGameEventListener() {}

    virtual void FireGameEvent(IGameEvent* event) = 0;
    virtual int GetEventDebugID(void) { return 42; };
};

class IGameEventManager
{
public:
    VFUNC(bool, addListener, 3, (IGameEventListener* listener, const char* name), (this, listener, name, false));
    VFUNC(void, removeListener, 5, (IGameEventListener* listener), (this, listener));
};
