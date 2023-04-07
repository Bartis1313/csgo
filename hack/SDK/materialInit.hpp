#pragma once

#include <mutex>
//#include <source_location> because somewhat __FUNCTON__ is still more accurate
#include <utilities/console/console.hpp>

// or create a class that does exactly same thing but add static keyword before object
#define INIT_MATERIALS_ONCE(func) \
{ \
    static auto name = __FUNCTION__; \
    static std::once_flag func##_flag; \
    std::call_once(func##_flag, [&]() { \
        func(); \
        console::debug("init function, location {}", name); \
    } ); \
}