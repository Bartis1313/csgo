#pragma once

#include <utilities/tools/tools.hpp>

#include <fstream>
#include <unordered_map>
#include <type_traits>

// generate netvar address
// type - template type for return type
// name - name your function
// table - table name to search the netvar prop
// prop - prop name
#define NETVAR(type, name, table, prop) \
[[nodiscard]] std::add_lvalue_reference_t<type> name() { \
	static uintptr_t offset = netvarMan.getNetvar(XOR(table), XOR(prop)); \
	return *reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset ); \
    }

// generate netvar, but address as the pointer
// type - template type for return type
// name - name your function
// table - table name to search the netvar prop
// prop - prop name
#define PTRNETVAR(type, name, table, prop) \
[[nodiscard]] std::add_pointer_t<type> name() { \
	static uintptr_t offset = netvarMan.getNetvar(XOR(table), XOR(prop)); \
	return reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset); \
    }

// define literal offset
// type - template type for return type
// name - name your function
// offset - offset to add
#define OFFSET(type, name, offset) \
[[nodiscard]] std::add_lvalue_reference_t<type> name() { \
	return *reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset); \
	}

// use like normal netvar, with bytes to add as last argument
// type - template type for return type
// name - name your function
// table - table name to search the netvar prop
// prop - prop name
// addr - extra offset to add
#define NETVAR_ADDR(type, name, table, prop, addr) \
[[nodiscard]] std::add_lvalue_reference_t<type> name() { \
	static uintptr_t offset = netvarMan.getNetvar(XOR(table), XOR(prop)); \
	return *reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset + addr); \
	}

class RecvTable;
class RecvProp;

class NetvarManager
{
public:
	void init();
	[[nodiscard]] uintptr_t getNetvar(const char* tableName, const char* propName) const;
	void dump();
private:
	std::unordered_map<std::string, RecvTable*> m_Tables;
	[[nodiscard]] uintptr_t getProp(const char* tableName, const char* propName, RecvProp** prop = nullptr) const;
	[[nodiscard]] uintptr_t getProp(RecvTable* recvTable, const char* propName, RecvProp** prop = nullptr) const;
	[[nodiscard]] RecvTable* getTable(const char* tableName) const;
	[[nodiscard]] std::string getType(RecvProp* recvTable) const;
	void dump(RecvTable* recvTable);
	std::ofstream file;
} inline netvarMan;