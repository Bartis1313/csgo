#pragma once

#include <vector>

// a base class for creating hacks, a constructor pushes the object into the vector and calls init on attach
// we should not care what is first as init or last, those things to init should not depend at all.
class BaseHack
{
public:
	static void initAll();
	static void shutdownAll();
protected:
	virtual void init() {};
	virtual void shutdown() {};
	inline static std::vector<BaseHack*> m_hacks;
};

#include <stdexcept>

class HackException : public std::runtime_error
{
public:
	void handleError(const HackException& err) const;
};