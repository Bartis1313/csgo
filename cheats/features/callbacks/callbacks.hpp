#pragma once

#include <classes/onlyInit.hpp>
#include <SDK/CClientEffectRegistration.hpp>

#include <vector>
#include <string>

class Callbacks : public OnlyInitType
{
public:
	constexpr Callbacks() :
		OnlyInitType{}
	{}

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