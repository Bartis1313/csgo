#pragma once

#include "helpers/vfunc.hpp"
#include "vars.hpp"

enum SoundFlags_t
{
	SND_NOFLAGS = 0,			// to keep the compiler happy
	SND_CHANGE_VOL = (1 << 0),		// change sound vol
	SND_CHANGE_PITCH = (1 << 1),		// change sound pitch
	SND_STOP = (1 << 2),		// stop the sound
	SND_SPAWNING = (1 << 3),		// we're spawning, used in some cases for ambients
	// not sent over net, only a param between dll and server.
	SND_DELAY = (1 << 4),		// sound has an initial delay
	SND_STOP_LOOPING = (1 << 5),		// stop all looping sounds on the entity.
	SND_SPEAKER = (1 << 6),		// being played again by a microphone through a speaker

	SND_SHOULDPAUSE = (1 << 7),		// this sound should be paused if the game is paused
	SND_IGNORE_PHONEMES = (1 << 8),
	SND_IGNORE_NAME = (1 << 9),		// used to change all sounds emitted by an entity, regardless of scriptname
	SND_IS_SCRIPTHANDLE = (1 << 10),		// server has passed the actual SoundEntry instead of wave filename

	SND_UPDATE_DELAY_FOR_CHOREO = (1 << 11),		// True if we have to update snd_delay_for_choreo with the IO latency.
	SND_GENERATE_GUID = (1 << 12),		// True if we generate the GUID when we send the sound.

	SND_OVERRIDE_PITCH = (1 << 13),		// The pitch given in code overrides what is present in the sound entry.
};

#define SND_FLAG_BITS_ENCODE 13

#define MAX_SOUND_INDEX_BITS	13
#define	MAX_SOUNDS				(1<<MAX_SOUND_INDEX_BITS)

// Todo: Morasky, we need to test against this #!
// MAX_MEASURED_SOUNDENTRIES

#if !defined( IN_XBOX_CODELINE )
// +/-4096 msec
#define MAX_SOUND_DELAY_MSEC_ENCODE_BITS	(13)
#else
// +/-65536 msec, 64 seconds
#define MAX_SOUND_DELAY_MSEC_ENCODE_BITS	(17)
#endif

// Subtract one to leave room for the sign bit
#define MAX_SOUND_DELAY_MSEC				(1<<(MAX_SOUND_DELAY_MSEC_ENCODE_BITS-1))    // 4096 msec or about 4 seconds

#define MAX_SOUND_SEED_BITS		6	// Used to encode 0-63 for seeding client side sound operators
									// NOTE: The LSB also carries the server side wave file selection
#define MIN_SOUND_SEED_VALUE	0
#define MAX_SOUND_SEED_VALUE	((1<<MAX_SOUND_SEED_BITS)-1)
#define MAX_SOUND_RNDWAVE_NUM	MAX_SOUND_SEED_VALUE

//-----------------------------------------------------------------------------
// common pitch values
//-----------------------------------------------------------------------------
#define	PITCH_NORM		100			// non-pitch shifted
#define PITCH_LOW		95			// other values are possible - 0-255, where 255 is very high
#define PITCH_HIGH		120

#define DEFAULT_SOUND_PACKET_VOLUME 1.0f
#define DEFAULT_SOUND_PACKET_PITCH	100
#define DEFAULT_SOUND_PACKET_DELAY	0.0f

//-----------------------------------------------------------------------------
// channels
//-----------------------------------------------------------------------------
enum
{
	CHAN_REPLACE = -1,
	CHAN_AUTO = 0,
	CHAN_WEAPON = 1,
	CHAN_VOICE = 2,
	CHAN_ITEM = 3,
	CHAN_BODY = 4,
	CHAN_STREAM = 5,		// allocate stream channel from the static or dynamic area
	CHAN_STATIC = 6,		// allocate channel from the static area 
	CHAN_VOICE_BASE = 7,		// allocate channel for network voice data
};

enum
{
	CHAN_USER_BASE = (CHAN_VOICE_BASE + 128)		// Anything >= this number is allocated to game code.
};

typedef unsigned int HSOUNDSCRIPTHASH;
#define SOUNDEMITTER_INVALID_HASH	(HSOUNDSCRIPTHASH)-1

class IEngineSound
{
public:
	VFUNC(void, stopSound, 8, (int entIndex, int channel, const char* sample, unsigned int soundEntryHash), (this, entIndex, channel, sample, soundEntryHash));
	VFUNC(void, emitAmbientSound, 12, (const char* sample, float volume, int pitch = PITCH_NORM, int flags = 0, float soundTime = 0.0f),
		(this, sample, volume, pitch, flags, soundTime));
};