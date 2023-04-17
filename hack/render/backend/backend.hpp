#pragma once

#include <windef.h>

struct IDirect3DDevice9;

// this is safe to use without checking if called once
namespace renderbackend
{
	void impl(IDirect3DDevice9* device);
	void createContext(HWND hwnd);
	void shutdown();
}