#include "hooks.hpp"

hooks::emitSound::value FASTCALL hooks::emitSound::hooked(FAST_ARGS, void* filter, int index, int channel,
	const char* name, uint32_t shash, const char* sample, float volume, int seed, int level,
	int flags, int pitch, const Vec3& origin, const Vec3& direction, void* vec, bool updatePos, float soundTime, int speakerEntity, void* soundParams)
{
	return original(thisptr, filter, index, channel,
		name, shash, sample, volume, seed, level,
		flags, pitch, origin, direction, vec, updatePos, soundTime, speakerEntity, soundParams);
}