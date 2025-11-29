#pragma once

#include "config.h"

#include <bass24/c/bass.h>

#include <cstdint>


class SoundEngine {
public:
	SoundEngine();
	void play_buffer(short* buffer);
	~SoundEngine();

private:
	static DWORD CALLBACK get_next_buffer(HSTREAM handle, int16_t* buffer, DWORD length, void* user);
};