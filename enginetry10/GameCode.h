#pragma once
#include "Helpers.h"
//handles the input, the sound, the rendering and the timing 


// Offscreen render buffer: stores pixel data in memory before it's displayed on the screen.
// Used for software rendering and double-buffering to avoid flicker.
struct Game_offscreen_buffer {
	void* BitMapMemory;
	int Width;
	int Height;
	int BytesPerPixel;
	int Pitch;
};




void 
GameUpdateAndRender(Game_offscreen_buffer *buffer, int BlueOffset, int GreenOffset);
