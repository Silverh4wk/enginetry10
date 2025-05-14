#include "GameCode.h"



internal void
GameOutputSound(Game_Sound_Output_Buffer*SoundBuffer, int ToneHz)
{
	local_persist real32 tSine;
	int16 Volume = 3000;

	int WavePeriod = SoundBuffer->SamplesPerSecond/ ToneHz;
	int16 *SampleOut = SoundBuffer->Samples;

	for (int SampleIndex = 0; SampleIndex < SoundBuffer->SampleCount; ++SampleIndex)
	{
		real32 SineValue = sinf(tSine) ;

		int16 SampleValue = (int16)(SineValue * Volume);
		*SampleOut++ = SampleValue;
		*SampleOut++ = SampleValue;
		tSine += 2.0f * Pi32 / (real32) WavePeriod;
	
	}
}

internal void
RenderCode(game_offscreen_buffer* Buffer, int BlueOffset, int GreenOffset)
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
			uint8 Green = (uint8)(Y + GreenOffset) ;
			uint8 Blue = (uint8)(Y + BlueOffset) ;
			uint8 Red = (uint8)(Y) ;
			//*Pixel++ = ((Blue << 8) | Green); // to fix the little endian change that was enforced by windows when writing to register 
					*Pixel++ = (Blue << 0) | (Green<< 8) | (Red<< 16);
			/*	uint8 Color = (uint8)(
					128.0f + (128.0f * (tan(X / 32.0f))) +
					128.0f + (128.0f * sin(Y / 8.0f)) +
					128.0f + (128.0f * sin((X  + GreenOffset) / 32.0f))
					) / 2;*/

					// BB GG RR AA
		}
		Row += Buffer->Pitch;
	}
}


void GameUpdateAndRender(game_memory *Memory,game_input* Input,
                    game_offscreen_buffer* buffer,
                    Game_Sound_Output_Buffer* SoundBuffer)
{
            Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
            game_state *GameState = (game_state*)Memory->PermanentStorage;
	 game_controller_input* Input0 = &Input->Controllers[0];

            if(!Memory->IsInit)
          {
           // char *FileName ="test.bmp";
            
            GameState->ToneHz = 256;
            GameState->GreenOffset = 0;
            GameState->BlueOffset = 0;
			Memory->IsInit = true;
          }
	 if (Input0->isAnalogue) 
	 {
		 GameState->ToneHz = 256 + (int)120.0f * (Input0->EndY);
		 GameState->BlueOffset += (int)4.0f * (Input0->EndY);
	 }
	 else
	 {
	 }
	 //Input0->Down.EndedDown;
	 //Input.AButtonHalfTransitionCount;

	 if (Input0->Down.EndedDown) {
		 GameState->BlueOffset -= 1;
	 }else if (Input0->Up.EndedDown) {
		 GameState->GreenOffset -= 1;
	 }
	

	 GameOutputSound(SoundBuffer,GameState->ToneHz);
	 RenderCode(buffer,GameState->BlueOffset,GameState-> GreenOffset);

}

