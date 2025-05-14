#if !defined(GAMECODE_H)
#include<math.h>
#include "Helpers.h"
//handles the input, the sound, the rendering and the timing 


// Offscreen render buffer: stores pixel data in memory before it's displayed on the screen.
// Used for software rendering and double-buffering to avoid flicker.


#if GAME_ITNERNAL
void *DEBUGPlatformReadEntireFile(char* FileName);
void  DEBUGPlatformFreeMem(void *Memory);

bool32 *DEBUGPlatformReadEntireFile(char* FileName, uint32 MemorySize, void * Memory);
#endif
struct game_offscreen_buffer {
	void* BitMapMemory;
	int Width;
	int Height;
	int BytesPerPixel;
	int Pitch;
};

struct game_button_state {
	int HalfTransitionCount;
	bool32 EndedDown;
};

struct game_controller_input {
	bool32 isAnalogue;
	real32 EndX, EndY;
	real32 StartX,StartY;
	real32 MinX,MinY;
	real32 MaxX,MaxY;

	union
	{
		game_button_state Buttons[8];
		struct {
			game_button_state Up;
			game_button_state Down;
			game_button_state Left;
			game_button_state Right;
			game_button_state LeftShoulder;
			game_button_state RightShoulder;
			game_button_state RightStick;
			game_button_state LeftStick;
		};

	};

};

struct game_input 
{
	game_controller_input Controllers[4];
};

struct game_state
{
  int ToneHz;
  int GreenOffset;
  int BlueOffset;
};

struct Game_Sound_Output_Buffer {
	int SampleCount;
	int SamplesPerSecond;
	int16* Samples;
};


struct game_clock 
{
	real32 SecondsElapsed;
};
struct game_memory
{
  bool32 IsInit;
  
  uint64 PermanentStorageSize;
  void *PermanentStorage;
  
  uint64 TransientStorageSize;
  void * TransientStorage;
};



void GameUpdateAndRender(game_memory* Memory, game_input* Input,
	game_offscreen_buffer* buffer, Game_Sound_Output_Buffer* SoundBuffer);
#define GameCode_H
#endif
