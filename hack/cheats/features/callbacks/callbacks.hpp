#pragma once

#include <cheats/classes/onlyInit.hpp>
#include <SDK/CClientEffectRegistration.hpp>

#include <vector>
#include <string>

class Callbacks : protected OnlyInitType
{
public:
	constexpr Callbacks() :
		OnlyInitType{}
	{}

	clientEffectCallback getCallbackByMemory(/*const */clientEffectCallback/*&*/ callb);
protected:
	virtual void init() override;
	virtual void shutdown() override;
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

GLOBAL_FEATURE(Callbacks);