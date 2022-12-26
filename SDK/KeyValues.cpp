#include "KeyValues.hpp"

#include "KeyValuesSys.hpp"
#include "interfaces/interfaces.hpp"

#include <gamememory/memory.hpp>

KeyValues::KeyValues(const char* key, const char* maybeSomePreValue, int caseSensitive)
{
	memory::keyValuesConstruct()(this, key, maybeSomePreValue, caseSensitive);
}

KeyValues::~KeyValues()
{
	// direct reference: [actual address in first opcode] E8 ? ? ? ? 8B 4B 14 5F 
	// probably casted sign or just int, from quick lookup seems 1 is default
	memory::keyValuesDestruct()(this, 1);
}

KeyValues* KeyValues::fromString(const char* name, const char* value)
{
	const static auto fromstring = memory::keyValuesFromString();
	KeyValues* res{};

	__asm
	{
		push 0
		mov edx, value
		mov ecx, name
		call fromstring
		add esp, 4
		mov res, eax
	}

	return res;
}

void KeyValues::fromBuffer(const char* name, const char* buffer, void* filesystem, const char* pathId, void* symbolProc, int unk)
{
	memory::loadFromBuffer()(this, name, buffer, filesystem, pathId, symbolProc, unk);
}

void* KeyValues::operator new(size_t size)
{
	return memory::interfaces::keyValuesSys->allocKeysMemory(size);
}