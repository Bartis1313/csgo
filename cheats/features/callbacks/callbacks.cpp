#include "callbacks.hpp"

#include <SDK/CEffectData.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/console/console.hpp>
#include <gamememory/memory.hpp>

void Callbacks::addCallBack(const clbStruct& callb)
{
    auto itr = std::find_if(m_callbacks.cbegin(), m_callbacks.cend(), [name = callb.m_name](const clbStruct& el)
        {
            return el.m_name == name;
        });

    if (itr != m_callbacks.cend())
        console.log(TypeLogs::LOG_WARN, XOR("{} already exists! skipping..."), callb.m_name);
    else
        m_callbacks.push_back(callb);
}

// custom1
void example(const CEffectData& data)
{ 
    auto call = g_Callbacks.getCallbackByMemory(&example);
    call(data);
}

// custom2 - fail
void example2(const CEffectData& data)
{
    auto call = g_Callbacks.getCallbackByMemory(&example2);
    call(data);
}

void Callbacks::init()
{
    this->addCallBack({ XOR("Impact"), &example, nullptr });
    this->addCallBack({ XOR("Impact"), &example2, nullptr });

    m_head = g_Memory.m_callbacksHead();

    for (auto head = m_head; head; head = head->m_next)
    {
        for (auto& el : m_callbacks)
        {
            // check by names on init, somehow we gotta point to correct name we passed
            if (std::string_view name = head->m_effectName; name == el.m_name)
            {
                el.m_callback = head->m_function; // own now the original function
                head->m_function = el.m_function; // point our edited function

                console.log(TypeLogs::LOG_INFO, XOR("custom function inited, name: {} at addr {:#0x}"),
                    name, reinterpret_cast<uintptr_t>(el.m_function));
            }
        }
    }
}

void Callbacks::shutdown()
{
    auto _head = m_head;
    for (auto head = _head; head; head = head->m_next)
    {
        for (auto& el : m_callbacks)
        {
            if (el.m_function == head->m_function) // check by address
                head->m_function = el.m_callback; // do not point to custom funcs anymore
        }
    }
}

clientEffectCallback Callbacks::getCallbackByMemory(/*const */clientEffectCallback/*&*/ callb)
{
    for (const auto& el : m_callbacks)
    {
        if (el.m_function == callb)
            return el.m_callback;
    }

    return nullptr;
}
