#pragma once

#include "../../classes/onlyInit.hpp"

#include <vector>
#include <string>

#include "../../../SDK/CClientEffectRegistration.hpp"

class Callbacks : public OnlyInitType
{
public:
	Callbacks()
	{
		m_hacks.push_back(this);
	}

	clientEffectCallback getCallbackByMemory(/*const */clientEffectCallback/*&*/ callb);
	virtual void init();
	virtual void shutdown();
private:
	struct clbStruct
	{
		std::string m_name; // effect's name
		clientEffectCallback m_function = nullptr; // init this on add as custom func
		clientEffectCallback m_callback = nullptr; // will be later inited
	};
	// Example of using it shown in cpp
	void addCallBack(const clbStruct& callb);

	std::vector<clbStruct> m_callbacks;
	CClientEffectRegistration* m_head;
};

[[maybe_unused]] inline auto g_Callbacks = Callbacks{};