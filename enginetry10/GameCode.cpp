#include "GameCode.h"

internal void
RenderCode(Game_offscreen_buffer* Buffer, int BlueOffset, int GreenOffset)
{
	Buffer->BytesPerPixel = 4;


	uint8* Row = (uint8*)Buffer->BitMapMemory;
	for (int Y = 0;
		Y < Buffer->Height;
		++Y) {
		uint32* Pixel = (uint32*)Row;
		for (int X = 0;
			X < Buffer->Width;
			++X)
		{
			uint8 Green = (uint8)(Y + GreenOffset);
			uint8 Blue = (uint8)(Y + BlueOffset);
			*Pixel++ = ((Blue << 8) | Green); // to fix the little endian change that was enforced by windows when writing to register 
			/*	uint8 Color = (uint8)(
					128.0f + (128.0f * (tan(X / 32.0f))) +
					128.0f + (128.0f * sin(Y / 8.0f)) +
					128.0f + (128.0f * sin((X  + GreenOffset) / 32.0f))
					) / 2;*/

					// BB GG RR AA
					//*Pixel++ = (Color << 0) | (Color << 8) | (Color << 16);
		}
		Row += Buffer->Pitch;
	}
}


 void GameUpdateAndRender(Game_offscreen_buffer* Buffer, int BlueOffset, int GreenOffset) {
	RenderCode(Buffer, BlueOffset, GreenOffset);
}

