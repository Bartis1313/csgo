#pragma once

using getSymbolProc_t = bool(__cdecl*)(const char*);

class KeyValues
{
public:
	// this is NOT allocing anything
	KeyValues() = default;
	KeyValues(const char* name);
	//void alloc(/*size_t size*/);
	//void free(/*void* mem*/);
	void* operator new(size_t size);
	void operator delete(void* mem);

	static KeyValues* fromString(const char* name, const char* value = nullptr);
	void loadFromBuffer(char const* resourceName, const char* buffer, void* fileSys = nullptr,
		const char* pathID = nullptr, getSymbolProc_t evaluateSymbolProc = nullptr);
};