#pragma once

#include <type_traits>

struct DataMap_t;

namespace netvars
{
	void init();
	void dump();

	[[nodiscard]] uintptr_t getNetvar(const std::string_view tableName, const std::string_view propName);
	[[nodiscard]] uintptr_t getDataMap(DataMap_t* map, const std::string_view name);
}

// generate netvar address
// type - template type for return type
// name - name your function
// table - table name to search the netvar prop
// prop - prop name
#define NETVAR(type, name, table, prop) \
[[nodiscard]] std::add_lvalue_reference_t<type> name() { \
	static uintptr_t offset = netvars::getNetvar(table, prop); \
	return *reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset ); \
	}

// generate netvar, but address as the pointer
// type - template type for return type
// name - name your function
// table - table name to search the netvar prop
// prop - prop name
#define PTRNETVAR(type, name, table, prop) \
[[nodiscard]] std::add_pointer_t<type> name() { \
	static uintptr_t offset = netvars::getNetvar(table, prop); \
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
	static uintptr_t offset = netvars::getNetvar(table, prop); \
	return *reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset + addr); \
	}

// use like normal netvar, with bytes to add as last argument, returns as pointer
// type - template type for return type
// name - name your function
// table - table name to search the netvar prop
// prop - prop name
// addr - extra offset to add
#define PTRNETVAR_ADDR(type, name, table, prop, addr) \
[[nodiscard]] std::add_pointer_t<type> name() { \
	static uintptr_t offset = netvars::getNetvar(table, prop); \
	return reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset + addr); \
	}

// generate datamap address
// type - template type for return type
// name - name your function
// table - table name to search the netvar prop
// prop - prop name
#define DATAMAP_FIELD(type, name, map, prop) \
[[nodiscard]] std::add_lvalue_reference_t<type> name() { \
	static uintptr_t offset = netvars::getDataMap(map, prop); \
	return *reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset ); \
	}

// generate netvar address as pointer
// type - template type for return type
// name - name your function
// table - table name to search the netvar prop
// prop - prop name
#define PTRDATAMAP_FIELD(type, name, map, prop) \
[[nodiscard]] std::add_pointer_t<type> name() { \
	static uintptr_t offset = netvars::getDataMap(map, prop); \
	return reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset ); \
	}