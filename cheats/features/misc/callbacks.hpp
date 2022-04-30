#pragma once

#include <vector>
#include <utility>
#include <string>
#include <functional>

#include "../../../SDK/CClientEffectRegistration.hpp"
#include "../../../SDK/math/Vector.hpp"

struct clbStruct
{
	std::string m_name; // effect's name
	clientEffectCallback m_function = nullptr; // init this on add as custom func
	clientEffectCallback m_callback = nullptr; // will be later inited
};

class Callbacks
{
public:
	// Example of using it shown in cpp
	void addCallBack(const clbStruct& callb);
	// init all addCallbacks here, before running.
	void init();
	void run();
	void shutdown();

	clientEffectCallback getCallbackByMemory(/*const */clientEffectCallback/*&*/ callb);
private:
	CClientEffectRegistration* getHead();
	std::vector<clbStruct> m_callbacks;
};

inline Callbacks callbacks;