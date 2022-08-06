#pragma once

#include <vector>

// a base class for creating hacks, see folder "classes" and features and example how to use it.
// we should not care what is first as init or last, those things to init should not depend at all.
class BaseHack
{
public:
	constexpr BaseHack() { m_hacks.push_back(this); }

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