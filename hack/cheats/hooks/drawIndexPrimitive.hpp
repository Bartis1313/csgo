#pragma once

#include <d3d9.h>

#include "helpers/common.hpp"

_HOOK_BEGIN
class DrawIndexedPrimitive
{
	ADD_HOOK_FRIEND;
private:
	using value = HRESULT;
	using call = value(D3DAPI*)(IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);

	static value D3DAPI hook(IDirect3DDevice9* device, D3DPRIMITIVETYPE primType, INT basevertexIndex, UINT minVertexIndex, UINT numVertices, UINT startIndex, UINT primCount);
	inline static call original;

	using clInit = std::function<void()>;
	using clRun = std::function<void(IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT)>;
protected:
	void registerInit(const clInit& callback) { Storage::inits.push_back(callback); }
	void registerRun(const clRun& callback) { Storage::runs.push_back(callback); }
	void registerShutdown(const clInit& callback) { Storage::shutdowns.push_back(callback); }
private:
	struct Storage
	{
		inline static vectorRunnable<clInit> inits;
		inline static vectorRunnable<clRun> runs;
		inline static vectorRunnable<clInit> shutdowns;
	};
};
_HOOK_END