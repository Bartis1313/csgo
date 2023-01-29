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
	static uintptr_t offset = netvarMan.getNetvar(table, prop); \
	return *reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset ); \
	}

// generate netvar, but address as the pointer
// type - template type for return type
// name - name your function
// table - table name to search the netvar prop
// prop - prop name
#define PTRNETVAR(type, name, table, prop) \
[[nodiscard]] std::add_pointer_t<type> name() { \
	static uintptr_t offset = netvarMan.getNetvar(table, prop); \
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

// define literal offset as pointer
// type - template type for return type
// name - name your function
// offset - offset to add
#define PTROFFSET(type, name, offset) \
[[nodiscard]] std::add_pointer_t<type> name() { \
	return reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset); \
	}

// use like normal netvar, with bytes to add as last argument
// type - template type for return type
// name - name your function
// table - table name to search the netvar prop
// prop - prop name
// addr - extra offset to add
#define NETVAR_ADDR(type, name, table, prop, addr) \
[[nodiscard]] std::add_lvalue_reference_t<type> name() { \
	static uintptr_t offset = netvarMan.getNetvar(table, prop); \
	return *reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset + addr); \
	}


#define PTRNETVAR_ADDR(type, name, table, prop, addr) \
[[nodiscard]] std::add_pointer_t<type> name() { \
	static uintptr_t offset = netvarMan.getNetvar(table, prop); \
	return reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset + addr); \
	}

#define DATAMAP_FIELD(type, name, map, prop) \
[[nodiscard]] std::add_lvalue_reference_t<type> name() { \
	static uintptr_t offset = netvarMan.getDataMap(map, prop); \
	return *reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset ); \
	}

#define PTRDATAMAP_FIELD(type, name, map, prop) \
[[nodiscard]] std::add_pointer_t<type> name() { \
	static uintptr_t offset = netvarMan.getDataMap(map, prop); \
	return reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset ); \
	}


class RecvTable;
class RecvProp;
struct DataMap_t;

class NetvarManager
{
public:
	void init();
	[[nodiscard]] uintptr_t getNetvar(const std::string_view tableName, const std::string_view propName) const;
	[[nodiscard]] uintptr_t getDataMap(DataMap_t* map, const std::string_view name) const;
	void dump();
private:
	std::unordered_map<std::string_view, RecvTable*> m_Tables;
	[[nodiscard]] uintptr_t getProp(const std::string_view tableName, const std::string_view propName, RecvProp** prop = nullptr) const;
	[[nodiscard]] uintptr_t getProp(RecvTable* recvTable, const std::string_view propName, RecvProp** prop = nullptr) const;
	[[nodiscard]] RecvTable* getTable(const std::string_view tableName) const;
	[[nodiscard]] std::string getType(RecvProp* recvTable) const;
	void dump(RecvTable* recvTable);
	std::ofstream file;
} inline netvarMan;