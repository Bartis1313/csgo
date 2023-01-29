#include "tools.hpp"

#include <gamememory/memory.hpp>

uintptr_t spoof::getGadget(const std::string_view moduleName, const std::initializer_list<uint8_t>& jmpRbxBytes)
{
    static std::unordered_map<std::string_view, void*> modulesPassed;

    if (const auto itr = modulesPassed.find(moduleName); itr == modulesPassed.end())
    {
        const auto _module = memory::getModule(moduleName);
        const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(_module);
        const auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uint8_t*>(_module) + dosHeader->e_lfanew);
        const auto section = IMAGE_FIRST_SECTION(ntHeaders);

        const auto addrStart = reinterpret_cast<uint8_t*>(_module) + section->VirtualAddress;
        const auto addrEnd = addrStart + section->Misc.VirtualSize;
        const auto jumpRBXaddr = std::ranges::search(addrStart, addrEnd, jmpRbxBytes.begin(), jmpRbxBytes.end());

        modulesPassed.emplace(std::make_pair(moduleName, jumpRBXaddr.data()));

        return reinterpret_cast<uintptr_t>(jumpRBXaddr.data());
    }
    else
    {
        return reinterpret_cast<uintptr_t>(itr->second);
    }
}