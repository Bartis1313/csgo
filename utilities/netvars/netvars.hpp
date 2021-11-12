#pragma once
#include <type_traits>
#include <cstdint>
#include <fstream>
#include <unordered_map>
#include "../../utilities/utilities.hpp"
#include "../../utilities/singleton.hpp"
#include "../../SDK/Recv.hpp"

// generate netvar, easy to find in client.dll
#define NETVAR(type, name, table, prop) \
_NODISCARD type& name() { \
	static uintptr_t offset = NetvarManager::g().getNetvar(XOR(table), XOR(prop)); \
	return *reinterpret_cast<type*>((uintptr_t)this + offset ); \
    }

// generate netvar, but address as the pointer
#define PTRNETVAR(type, name, table, prop) \
_NODISCARD type* name() { \
	static uintptr_t offset = NetvarManager::g().getNetvar(XOR(table), XOR(prop)); \
	return reinterpret_cast<type*>((uintptr_t)this + offset); \
    }

// define literal offset
#define OFFSET(type, name, offset) \
_NODISCARD type& name() { \
	return *reinterpret_cast<type*>((uintptr_t)this + offset); \
	}

// use like normal netvar, with bytes to add as last argument
#define NETVAR_ADDR(type, name, table, prop, addr) \
_NODISCARD type& name() { \
	static uintptr_t offset = NetvarManager::g().getNetvar(XOR(table), XOR(prop)); \
	return *reinterpret_cast<type*>((uintptr_t)this + offset + addr); \
	}

class NetvarManager : public singleton<NetvarManager>
{
public:
	void init();
	uintptr_t getNetvar(const char* tableName, const char* propName);
	void dump(RecvTable* recvTable);
	void dump();
private:
	std::unordered_map<std::string, RecvTable*> m_Tables;
	uintptr_t getProp(const char* tableName, const char* propName, RecvProp** prop = 0);
	uintptr_t getProp(RecvTable* recv, const char* propName, RecvProp** prop = 0);
	RecvTable* getTable(const char* tableName);
	std::ofstream file;
};