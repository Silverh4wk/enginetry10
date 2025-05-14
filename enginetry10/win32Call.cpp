/*
ok how do i write the documentation for this thing ugh do i really have to do this 
ok so first talk abt the headers and the special type defs that i have going on 
--> getWindowDimensions() 
--> XAudioClass::XAudioClass()
--> XAudioClass::XAudioClass(const XAudioClass& )
--> XAudioClass::~XAudioClass()
--> win32_LoadXInput()
--> bool XAudioClass::Initialize()
-->RenderCode(win32_offscreen_buffer *Buffer,int BlueOffset, int GreenOffset)
-->Win32_ResizeDIBSection(win32_offscreen_buffer *Buffer,int Height, int Width) 
-->win32InitSound(HWND Window,int32 SamplesPerSecond,int32 BufferSize)
-->win32_DisplayBufferInWindow(win32_offscreen_buffer Buffer ,HDC DeviceContext,int Width, int Height)
-->switch (Message)
-->Keyboard cases
-->Painting of the application 
-->win32XAudioInit()
-->
*/
//NOTE(Hazim) :  Remeber to update those comments 

/*
         CHECKLIST

-save game location 
-getting a handle to our own exe file 
-asset loading path 
-threading 
-raw input 
-sleep and time begin period 
-clipcursor for multimonitor support and full screen support 
-WM_SETCURSOR (Invinsible)
-WM_ACTIVATEAPP 
-blitspeed improvement 
-hardware accel 

*/

#include "Helpers.h"
#include "GameCode.h"
#include <windows.h>
#include <malloc.h>
#include "DataStructures.h"
#include <Xinput.h>
#include <dsound.h>
#include "XAudioClass.h"
#include "XAudioSoundClass.h"
#include <math.h>


//A library-search record in the object file.
#pragma comment(lib, "dsound.lib") // DirectSound that i may later update to xAudio
#pragma comment(lib, "dxguid.lib")// 
#pragma comment(lib, "winmm.lib")// windows mulitmedia wrapper 


struct win32_WindowDimensions {
	int Width;
	int Height;
};

struct win32_offscreen_buffer {
	BITMAPINFO BitMapInfo;
	void* BitMapMemory;
	int Width;
	int Height;
	int BytesPerPixel;
	int Pitch;
};

struct dsound_Sound_Output 
{
	//Direct Sound Audio data 
	int Volume ;
	int SamplerPerSecond ;
	uint32 RunningSampleIndex ;
	int BytesPerSample ;
	int  BufferSize;
	int LatencySampleCount;
};



XAudioClass::XAudioClass() {
	XAudio2 = 0;
	MasterVoice = 0;
}

XAudioClass::XAudioClass(const XAudioClass& other)
{
	XAudio2 = 0;
	MasterVoice = 0;
}


XAudioClass::~XAudioClass()
{
}

bool XAudioClass::Initialize()
{
	HRESULT result;
	// Initialize COM first.
	result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(result))
	{
		return false;
	}
	//Creating of our XAudio instance
	result = XAudio2Create(&XAudio2, 0, XAUDIO2_USE_DEFAULT_PROCESSOR);
	if (FAILED(result))
	{
		return false;
	}
	//the final output stage in the audio graph that then sends the data to our audio output device
	result = XAudio2->CreateMasteringVoice(&MasterVoice, 
		XAUDIO2_DEFAULT_CHANNELS,XAUDIO2_DEFAULT_SAMPLERATE,
		0,NULL);
	// result = XAudio2->CreateMasteringVoice(&MasterVoice); -- this too can work i suppose 
	if (FAILED(result)) 
	{
		return false;
	}

	return true;

}
//releasing of resources of XAudio
void XAudioClass::Shutdown()
{
	// Release the master voice.
	if (MasterVoice)
	{
		MasterVoice->DestroyVoice();
		MasterVoice = 0;
	}

	// Release the Xaudio2 interface.
	if (XAudio2)
	{
		XAudio2->Release();
		XAudio2 = 0;
	}

	// Uninitialize COM.
	CoUninitialize();

	return;
}

IXAudio2* XAudioClass::GetXAudio2()
{
	return XAudio2;
}

typedef DWORD WINAPI XInput_Get_State
(
	DWORD         dwUserIndex,  // Index of the GameController associated with the device
	XINPUT_STATE* pState        // Receives the current state
);
typedef DWORD WINAPI XInput_Set_State
(
	DWORD             dwUserIndex,  // Index of the GameController associated with the device
	XINPUT_VIBRATION* pVibration    // The vibration information to send to the controller
) ;

// ==== XInput Setup for Controller Input ====
//
// Macro to define the function signature for XInputGetState
// (used to query the current state of an Xinput Controller).

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState )

// Typedef for a function pointer to DirectSoundCreate.
// Used for dynamic loading of the Xinput library
typedef X_INPUT_GET_STATE(X_Input_Get_state);

// Stub fallback for XInputGetState — returns 0 (no input).
// This is used if the real XInput function isn't found at runtime.

X_INPUT_GET_STATE (XInputGetStateStub) { return(0); }

// Global function pointer to XInputGetState.
// Initialized to point to the stub by default. Will be replaced
// with the real function if loaded dynamically.

global_variable X_Input_Get_state *XInputGetstate_ = XInputGetStateStub;
#define XInputGetState XInputGetstate_

#define X_INPUT_SET_STATE(name) DWORD  WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)

// Typedef for a function pointer to DirectSoundCreate.
// Used for dynamic loading of the Xinput library
typedef X_INPUT_SET_STATE(X_Input_Set_State);
//Same for this one
X_INPUT_SET_STATE(XInputSetStateStub) { return(0); }
global_variable X_Input_Set_State *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_


// ==== DirectSound Setup for Audio Output ====

//
// Macro to define the function signature for DirectSoundCreate,
// which initializes a DirectSound object.
//
#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter);

// Typedef for a function pointer to DirectSoundCreate.
// Used for dynamic loading of the DirectSound library.
typedef DIRECT_SOUND_CREATE (direct_sound_create);


internal void *DEBUGPlatformLoadFile(char* FileName)
{
  void*Result;
  //create file
  HANDLE FileHandle = CreateFileW((LPCWSTR)FileName,GENERIC_READ,FILE_SHARE_READ, 0, OPEN_EXISTING,0,0);
  if(FileHandle != INVALID_HANDLE_VALUE)
  {
    LARGE_INTEGER FileSize;
    if(GetFileSizeEx(FileHandle,&FileSize))
      {
        Assert(FileSize.QuadPart <= 0xFFFFFFFF);			
       Result=VirtualAlloc,(0,FileSize.QuadPart,MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        if(Result)
        {
          DWORD BytesRead = (DWORD)strlen(FileName);
          if (ReadFile(FileHandle, Result, FileSize.QuadPart, &BytesRead,0))
                     {
                       
                     }
          else{}
        }
        CloseHandle(FileHandle);
      }
    return (Result);
  }
  //filesize
  //closehandle;
  //readfile
}

internal void  DEBUGPlatformFreeMem(void *Memory)
{
  if(Memory)
  {
    VirtualFree(Memory,0, MEM_RELEASE);
  }
}

internal bool32 *DEBUGPlatformReadEntireFile(char* FileName,
                                             uint32 MemorySize, void * Memory)
{
  
}
//Loading of the Xinput library, if it fails, it will just default to the stub 
internal void 
win32_LoadXInput(void) {
	HMODULE Xinputlibrary = LoadLibraryA("xinput1_3.dll");
	if (Xinputlibrary) 
	{
		XInputGetState = (X_Input_Get_state*)GetProcAddress(Xinputlibrary,"XInputGetState");
		XInputSetState = (X_Input_Set_State*)GetProcAddress(Xinputlibrary, "XInputSetState");
	}
}

internal void
win32_ProcessDigitalXinputButtonState(DWORD XINPUTBUTTONSTATE,
	game_button_state* OldState,DWORD ButtonBit 
	, game_button_state* NewState) 
{
	NewState->EndedDown = ((XINPUTBUTTONSTATE & ButtonBit) == ButtonBit);
	NewState->HalfTransitionCount != (OldState->EndedDown != NewState->EndedDown) ? 1:0;
}
//A function to grab the dimensions of the passed thro window handle and return an object
internal win32_WindowDimensions 
GetWindowDimensions(HWND Window) {
	win32_WindowDimensions dimensions;
	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	dimensions.Width = ClientRect.right - ClientRect.left;
	dimensions.Height = ClientRect.bottom - ClientRect.top;
	return(dimensions);
}

// ==== Global Variables ====
//
global_variable bool GlobalRunning = 0; // The state of the application
global_variable win32_offscreen_buffer BackBuffer; // OffScreen render buffer
global_variable LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer; //Audio mixing happens here
global_variable dsound_Sound_Output SoundOutput = {};

// ==== Stub Rendering Code ====
//
// Renders a simple gradient pattern into an offscreen buffer.
// The output is written to `Buffer->BitMapMemory` , which holds raw pixel data.
//
// BlueOffset and GreenOffset control the (X for horizontal and Y for Vertical ) color gradient shift.
//
// Note: Assumes 32-bit (4 bytes per pixel) pixel format. Pixels are written
// as 0x00BBGGRR due to little-endian byte order on Windows.
// Which is fixed by shifting by 8 bits every time a row is drawn.



//
// Resizes the offscreen buffer (DIB section: the color table) 
// 
// Parameters:
// - Buffer: The offscreen buffer to be resized.
// - Width, Height: New dimensions for the buffer.
// https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader --> for more info 

internal void
Win32_ResizeDIBSection(win32_offscreen_buffer* Buffer, int Height, int Width)
{
	// Free existing memory if found.
	if (Buffer->BitMapMemory)
	{
		VirtualFree(Buffer->BitMapMemory, 0, MEM_RELEASE);
	}
	// else 
	// Update the buffer's dimensions.
	Buffer->Width = Width;
	Buffer->Height = Height;
	Buffer->BytesPerPixel = 4; // 32-bit color ---> [BB][GG][RR][XX] 

	// Fill out the BITMAPINFO header for the new bitmap.
	BITMAPINFOHEADER* Header = &Buffer->BitMapInfo.bmiHeader;
	Header->biSize = sizeof(*Header);   // Required size of the header.
	Header->biWidth = Buffer->Width;     // Width in pixels.
	Header->biHeight = -Buffer->Height;   // Negative to indicate a top-down DIB (origin is top-left).
	Header->biPlanes = 1;                 // Always 1 for bitmaps.
	Header->biBitCount = 32;                // 32 bits per pixel (no palette).
	Header->biCompression = BI_RGB;            // Uncompressed RGB (no color compression).

	// Calculate the total size of the bitmap in bytes.
	int BitMapMemorySize = Buffer->BytesPerPixel * (Buffer->Width * Buffer->Height);

	// Allocate memory for the pixel buffer using VirtualAlloc.
	// PAGE_READWRITE allows read/write access.
	Buffer->BitMapMemory = VirtualAlloc(0, BitMapMemorySize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	// Pitch = number of bytes per row of pixels.
	// Important when traversing rows in memory during rendering.
	Buffer->Pitch = Width * Buffer->BytesPerPixel;
}

// Redrawing of a copied color to the destinatino window 
// 
// If destination is larger than the copied colors, its stretched to 
// the size of the destination window and the opposite is true (compression)
//https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-stretchdibits

internal void 
win32_DisplayBufferInWindow(win32_offscreen_buffer *Buffer ,HDC DeviceContext,int Width, int Height){
	StretchDIBits(
		DeviceContext,  
		0,0,Width,Height,
		0,0, Buffer->Width, Buffer->Height,
		Buffer->BitMapMemory,
		&Buffer->BitMapInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);

}
// ===  Main Window call back ===
//
// Processing of messages that are sent to the window
// 
//
LRESULT CALLBACK 
MainWindowCallBack(HWND Window,
	UINT   Message,
	WPARAM wParam,
	LPARAM lParam) 
{
	LRESULT Result = 0;
	//To be filled later, rn is used mostly for debugging 
	// and checking if those messages are actually getting handled properly
    

	switch (Message) 
	{
	case WM_SIZE:
	{
		OutputDebugStringA("WM_SIZE\n");
	}
		break;
	case WM_DESTROY:
		GlobalRunning = 0;
		OutputDebugStringA("WM_DESTROY\n");
		break;
	case WM_CLOSE:
		GlobalRunning = 0;
		    OutputDebugStringA("WM_CLOSE\n");
		break;
	case WM_ACTIVATEAPP:
		OutputDebugStringA("WM_ACTIVATEAPP\n");
		break;
	
	
	// Handles keyboard input from Windows messages.
	//
	// Supports both system (WM_SYSKEY*) and regular key events (WM_KEY*).
	// Extracts key state information from lParam:
	//   - WasDown: true if the key was previously down (used to detect repeats)
	//   - IsDown: true if the key is currently pressed
	//   - AltKeyIsDown: true if the ALT key was held when the event fired
	// (Note: Hazim) -- Much more work need to be done here 

	case WM_SYSKEYDOWN: 
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{

		uint32 VKCode = (uint32)wParam;
		bool WasDown = ((lParam & (1 << 30)) != 0);
		bool IsDown = ((lParam & (1 << 31)) == 0);
		bool AltKeyIsDown = ((lParam & (1 << 29)) != 0);
			if (VKCode == 'W') {
				OutputDebugStringA("w\n");
			}
			else if (VKCode == 'A') {
				OutputDebugStringA("A\n");
			}
			else if (VKCode == 'S') {
				OutputDebugStringA("S\n");

			}
			else if (VKCode == 'D') {
				OutputDebugStringA("D\n");

			}
			else if (VKCode == VK_LSHIFT) {}
			else if (VKCode == VK_SPACE) {}
			else if (VKCode == VK_ESCAPE) {
			
			}
			else if (VKCode == VK_RETURN) {
				OutputDebugStringA("RETURN: ");
				if (IsDown) { OutputDebugStringA("is being held down "); }
				if (WasDown) { OutputDebugStringA("Released "); }
				OutputDebugStringA("\n");
			}
			else if ((VKCode == VK_F4 ) && AltKeyIsDown) {
				GlobalRunning = false;

			}

	}
		break;

	// Handles the drawing messages that are sent through 

	case WM_PAINT:
	{
		
		PAINTSTRUCT Paint;
		HDC DeviceContext = BeginPaint(Window, &Paint);
		int x = Paint.rcPaint.left;
		int y = Paint.rcPaint.top;
		LONG Height = Paint.rcPaint.bottom  -  Paint.rcPaint.top;
		LONG Width = Paint.rcPaint.right  - Paint.rcPaint.left;
		win32_WindowDimensions D = GetWindowDimensions(Window);
		win32_DisplayBufferInWindow(&BackBuffer, DeviceContext, D.Width, D.Height);
		EndPaint(Window, &Paint);
		}
	default: {
		//OutputDebugStringA("DEFAULT\n");
		Result = DefWindowProcA(Window, Message, wParam, lParam);
		}
		break;
	}
	return(Result);
}
/*
one is creating the buffer the other is getting the handle to the soundcard
We write things into the secondary buffer 


*/
// Will definetly be updated to Xaudio later
//Load dsound.dll
//get a pointer to a single function and we call that function to give us back the address of the object
//if the create of directsound succeed, fill up the waveformate with needed data,,,
// just check https://learn.microsoft.com/en-us/previous-versions/windows/desktop/ee419019(v=vs.85)
//using the vtable that got mapped into memory by loading the dll, we can get the rest of the functions to call right of the object itself
//two buffers --> only using one of them since windows doesnt allow us to direcly right to the primary buffer
// the primary buffer call is just used to get the handle of the sound card which is then used in the secondary buffer

internal void
win32InitSound(HWND Window,int32 SamplesPerSecond,int32 BufferSize)
{
	HMODULE DSoundlibrary = LoadLibraryA("dsound.dll");
	if (DSoundlibrary) {
		direct_sound_create * DirectCreateSound = (direct_sound_create*)GetProcAddress(DSoundlibrary,"DirectSoundCreate");
		LPDIRECTSOUND directsound;
		if (DirectCreateSound && SUCCEEDED(DirectSoundCreate (0,&directsound,0))) {
			WAVEFORMATEX WaveFormateX;
			WaveFormateX.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormateX.nChannels = 2;
			WaveFormateX.wBitsPerSample = 16;
			WaveFormateX.nSamplesPerSec = SamplesPerSecond;
			WaveFormateX.nBlockAlign = (WaveFormateX.nChannels * WaveFormateX.wBitsPerSample) / 8;
			WaveFormateX.nAvgBytesPerSec = (SamplesPerSecond * WaveFormateX.nBlockAlign);
			WaveFormateX.cbSize = 0;
			if(SUCCEEDED(directsound->SetCooperativeLevel(Window, DSSCL_PRIORITY))){
				DSBUFFERDESC BufferDescription = {};
				memset(&BufferDescription, 0, sizeof(DSBUFFERDESC));
				BufferDescription.dwSize = { sizeof(BufferDescription) };
				BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
				BufferDescription.dwBufferBytes= 0;
				LPDIRECTSOUNDBUFFER PrimaryBuffer = NULL;
				HRESULT Error = directsound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, NULL);
				HRESULT SetFromatError = PrimaryBuffer->SetFormat(&WaveFormateX);
				if (SUCCEEDED(Error)) {
					if (SUCCEEDED(SetFromatError))
					{
						OutputDebugStringA("Primary Buffer\n");
					}
					else
					{
					}
				}
				else {}
				DSBUFFERDESC BufferDescription2 = {};
				BufferDescription2.dwSize = { sizeof(BufferDescription2) };
				BufferDescription2.dwBufferBytes = BufferSize;
				BufferDescription2.dwFlags = 0;
				BufferDescription2.lpwfxFormat = &WaveFormateX;
				LPDIRECTSOUNDBUFFER SecondaryBuffer = NULL;
				Error = directsound->CreateSoundBuffer(&BufferDescription2, & GlobalSecondaryBuffer, NULL);
				if (SUCCEEDED(Error))
				{
					OutputDebugStringA("Secondary Buffer\n");
				}
					else
					{

					}
			}
			else{}
		}
		else {}

	}

}

internal void
win32XAudioInit() {
	XAudio2Create(
		0, 0, XAUDIO2_DEFAULT_PROCESSOR
	);

}

//Application Entry point
// Paramters -
//   HINSTANCE Instance, - A handle to the current instance of the application.
//   HINSTANCE PrevInstance, - A handle to the previoous instance of the application - ALWAYS NULL
//   LPSTR     CmdLine, - The command line for the application
//   int       ShowCode - controls how the window is shown

internal void
win32ClearBuffer(dsound_Sound_Output* SoundOutput) {
	
	VOID* Region;
	DWORD RegionSize;
	VOID* Region2;
	DWORD Region2Size;
	HRESULT hr = GlobalSecondaryBuffer->Lock(0, SoundOutput->BufferSize ,
		&Region, &RegionSize,
		&Region2, &Region2Size,
		0);
	if (SUCCEEDED(hr))
	{

		DWORD RegionSampleCount = RegionSize / SoundOutput->BytesPerSample;
		DWORD Region2SampleCount = Region2Size / SoundOutput->BytesPerSample;
		uint8* DestSample = (uint8*)Region;
		for (DWORD ByteIndex = 0;  ByteIndex < RegionSize; ++ByteIndex)
		{
			*DestSample++ = 0;
		}
		DestSample = (uint8*)Region2;
		for (DWORD ByteIndex = 0; ByteIndex < Region2Size; ++ByteIndex)
		{
			*DestSample++ = 0;

		}
		hr = GlobalSecondaryBuffer->Unlock(Region, RegionSize, Region2, Region2Size);
	}
}

internal void 
win32FillSoundBuffer(dsound_Sound_Output* SoundOutput,
	DWORD ByteToLock, DWORD WriteBytes,
	Game_Sound_Output_Buffer *SoundBuffer)
{
	VOID* Region;
	DWORD RegionSize;
	VOID* Region2;
	DWORD Region2Size;
	HRESULT hr = GlobalSecondaryBuffer->Lock(ByteToLock, WriteBytes,
		&Region, &RegionSize,
		&Region2, &Region2Size,
		0);
	if (SUCCEEDED(hr))
	{

		DWORD RegionSampleCount = RegionSize / SoundOutput->BytesPerSample;
		DWORD Region2SampleCount = Region2Size / SoundOutput->BytesPerSample;
		int16*  DestSample  = (int16*)Region;
		int16* SourceSample = SoundBuffer->Samples;
		for (DWORD SampleIndex = 0; SampleIndex < RegionSampleCount; ++SampleIndex)
		{
			*DestSample++ = *SourceSample++;
			*DestSample++ = *SourceSample++;
			++SoundOutput->RunningSampleIndex;
		}
		DestSample = (int16*)Region2;
		for (DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; ++SampleIndex)
		{
			*DestSample++ = *SourceSample++;
			*DestSample++ = *SourceSample++;
			++SoundOutput->RunningSampleIndex;

		}
		hr = GlobalSecondaryBuffer->Unlock(Region, RegionSize, Region2, Region2Size);
	}
}


int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, int showCode)
{

	LARGE_INTEGER PerfCounterFrequencyResult;
	QueryPerformanceFrequency(&PerfCounterFrequencyResult);
	int64 PerfCounterFrequency = PerfCounterFrequencyResult.QuadPart;

	win32_LoadXInput();
	WNDCLASSEXA WindowClass = { 0 };
	Win32_ResizeDIBSection(&BackBuffer, 1280, 720);



	WindowClass.cbSize = sizeof(WindowClass);
	WindowClass.style = CS_OWNDC | CS_HREDRAW; // flags --> A unique device context and complete redraw evertime the window is adjusted
	WindowClass.lpfnWndProc = MainWindowCallBack; // will handle all of the messages that are coming from windows 
	WindowClass.hInstance = Instance; // handle the instance of the window 
	WindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowClass.hbrBackground = NULL;
	WindowClass.lpszClassName = "ScratchGameWindowClass";

	//if true, create Window
	//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassa
	DWORD dw = GetLastError();

	if (RegisterClassExA(&WindowClass)) {
		HWND Window = CreateWindowExA(
			0,
			WindowClass.lpszClassName,
			"TestApplication",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			Instance,
			0
		);
		if (Window)
		{
			LARGE_INTEGER BeginCounter;
			QueryPerformanceCounter(&BeginCounter);
			BeginCounter.QuadPart;

			//graphics data 
			HDC DeviceContext = GetDC(Window);

			//dsound audio info 
			dsound_Sound_Output SoundOutput = {};
			SoundOutput.SamplerPerSecond = 48000;
			SoundOutput.RunningSampleIndex = 0;
			SoundOutput.BytesPerSample = sizeof(int16) * 2;
			SoundOutput.BufferSize = SoundOutput.SamplerPerSecond * SoundOutput.BytesPerSample;
			SoundOutput.LatencySampleCount = SoundOutput.SamplerPerSecond / 15;
			win32InitSound(Window, SoundOutput.SamplerPerSecond, SoundOutput.BufferSize);
			win32ClearBuffer(&SoundOutput);
			GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

			int16* Samples = (int16*)VirtualAlloc(0, SoundOutput.BufferSize,
				MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

#if Game_Internal
			LPVOID BaseAddress = (LPVOID)TeraBytes((uint64)2);
#else 
			LPVOID BaseAddress = 0;
#endif
			game_memory GameMemory = {};
			GameMemory.PermanentStorageSize = MegaBytes(64);
			GameMemory.TransientStorageSize = GigaBytes((uint64)4);
			uint64 TotalSize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;
			GameMemory.PermanentStorage = VirtualAlloc(BaseAddress, TotalSize,
				MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			GameMemory.TransientStorage = ((uint8*)GameMemory.PermanentStorage +
				GameMemory.PermanentStorageSize);
			//xaudio audio info 
			XAudioClass* XAudioClass_ = 0;
			XAudioClass_ = new XAudioClass;
			XAudioSoundClass* TestSound1;
			char soundFilename[128];


			HRESULT result;
			result = XAudioClass_->Initialize();
			if (FAILED(result))
			{
				MessageBoxW(Window, L"Could not initialize XAudio.", L"Error", MB_OK);
				return result;
			}

			TestSound1 = new XAudioSoundClass;

			strcpy_s(soundFilename, "01_Children_of_the_Omnissiah.wav");

			result = TestSound1->LoadTrack(XAudioClass_->GetXAudio2(), soundFilename, 1.0f);
			if (FAILED(result))
			{
				MessageBoxW(Window, L"Could not initialize test sound object.", L"Error", MB_OK);
				return result;
			}
			/*result = TestSound1->PlayTrack();
			if (FAILED(result)) {
				MessageBoxW(Window, L"Could not Play test sound object.", L"Error", MB_OK);
				return result;
			}*/

			if (Samples && GameMemory.PermanentStorage && GameMemory.TransientStorage) {
				LARGE_INTEGER LastCounter;
				QueryPerformanceCounter(&LastCounter);

				int64 LastCycleCount = __rdtsc();

				game_input Input[2] = {};

				game_input* NewInput = &Input[0];
				game_input* OldInput = &Input[1];

				GlobalRunning = true;
				while (GlobalRunning)
				{

					MSG Message;

					//Peek at the message queue, translate and then dispatch them 
					//Peek is used instead of GetMessage as it doesnt "clog" the queue and allow for contant feeding
					while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
					{

						if (Message.message == WM_QUIT)
						{
							GlobalRunning = false;
							//Release the test sound object.
							if (TestSound1)
							{
								// Stop the sound if it was still playing.
								TestSound1->StopTrack();

								// Release the test sound object.
								TestSound1->ReleaseTrack();
								delete TestSound1;
								TestSound1 = 0;
							}

							// Release the XAudio object.
							if (XAudioClass_)
							{
								XAudioClass_->Shutdown();
								delete XAudioClass_;
								XAudioClass_ = 0;
							}

						}

						TranslateMessage(&Message);
						DispatchMessageA(&Message);
					}

					//loop thro the connected controllers and get their inputs, if found any 

					int MaxControllerCount = XUSER_MAX_COUNT;
					if (MaxControllerCount > ArrayCount(NewInput->Controllers))
					{
						MaxControllerCount = ArrayCount(NewInput->Controllers);
					}
					for (DWORD ControllerIndex = 0;
						ControllerIndex < MaxControllerCount;
						++ControllerIndex)
					{
						game_controller_input* OldController = &OldInput->Controllers[ControllerIndex];
						game_controller_input* NewController = &NewInput->Controllers[ControllerIndex];
						XINPUT_STATE ControllerState;
						if (XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
						{
							XINPUT_GAMEPAD* GamePad = &ControllerState.Gamepad;

							bool32 Start = (GamePad->wButtons & XINPUT_GAMEPAD_START);
							bool32 Back = (GamePad->wButtons & XINPUT_GAMEPAD_BACK);

							bool32 DpadUp = (GamePad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
							bool32 DpadDown = (GamePad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
							bool32 DpadLeft = (GamePad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
							bool32 DpadRight = (GamePad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

							int16 StickX = (real32)GamePad->sThumbLX;
							int16 StickY = (real32)GamePad->sThumbLY;

							NewController->isAnalogue = true;
							NewController->StartX = OldController->EndX;
							NewController->StartY = OldController->EndY;

							real32 X = 0; real32 Y =0;
							if (GamePad->sThumbLX < 0)
							{
								NewController->MinX = NewController->MaxX = NewController->EndX = X;
							}
								if (GamePad->sThumbLY < 0)
								{
									Y = (real32)GamePad->sThumbLY / -32768.0f;
								}
								else
								{
									Y = (real32)GamePad->sThumbLY / 32767.0f;
								}
								NewController->MinY = NewController->MaxY = NewController->EndY = Y;
								win32_ProcessDigitalXinputButtonState(GamePad->wButtons,
									&OldController->Down, XINPUT_GAMEPAD_A, &NewController->Down);

								win32_ProcessDigitalXinputButtonState(GamePad->wButtons,
									&OldController->Right, XINPUT_GAMEPAD_B, &NewController->Right);

								win32_ProcessDigitalXinputButtonState(GamePad->wButtons,
									&OldController->Left, XINPUT_GAMEPAD_X, &NewController->Left);

								win32_ProcessDigitalXinputButtonState(GamePad->wButtons,
									&OldController->Up, XINPUT_GAMEPAD_Y, &NewController->Up);

								win32_ProcessDigitalXinputButtonState(GamePad->wButtons,
									&OldController->LeftStick, XINPUT_GAMEPAD_LEFT_THUMB, &NewController->LeftStick);

								win32_ProcessDigitalXinputButtonState(GamePad->wButtons,
									&OldController->RightStick, XINPUT_GAMEPAD_RIGHT_THUMB, &NewController->RightStick);

								win32_ProcessDigitalXinputButtonState(GamePad->wButtons,
									&OldController->LeftShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER, &NewController->LeftShoulder);

								win32_ProcessDigitalXinputButtonState(GamePad->wButtons,
									&OldController->RightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER, &NewController->RightShoulder);







								XINPUT_VIBRATION Vibration;
								Vibration.wLeftMotorSpeed = 2;
								Vibration.wRightMotorSpeed = 2;
								//XInputSetState(0, &Vibration);
								if (DpadLeft) {
								}
								if (DpadRight) {
								}
								if (DpadDown) {
								}
								if (DpadUp) {

								}

							
							else
							{
							}

						}

						DWORD ByteToLock = 0;
						DWORD TargetCursor = 0;
						DWORD WriteBytes = 0;
						DWORD WriteCursor = 0;
						DWORD PlayCursor = 0;
						boolean SoundIsValid = false;
						HRESULT hr = GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor);
						if (SUCCEEDED(hr))
						{
							ByteToLock = (SoundOutput.RunningSampleIndex * SoundOutput.BytesPerSample) % SoundOutput.BufferSize;

							TargetCursor = (PlayCursor + SoundOutput.LatencySampleCount * SoundOutput.BytesPerSample) %
								SoundOutput.BufferSize;

							if (ByteToLock > TargetCursor)
							{
								WriteBytes = SoundOutput.BufferSize - ByteToLock;
								WriteBytes += TargetCursor;
							}
							else
							{
								WriteBytes = TargetCursor - ByteToLock;

							}
							SoundIsValid = true;
						}

						Game_Sound_Output_Buffer SoundBuffer = {};
						SoundBuffer.SamplesPerSecond = SoundOutput.SamplerPerSecond;
						SoundBuffer.SampleCount = WriteBytes / SoundOutput.BytesPerSample;
						SoundBuffer.Samples = Samples;



						game_offscreen_buffer Buffer = {};
						Buffer.BitMapMemory = BackBuffer.BitMapMemory;
						Buffer.Width = BackBuffer.Width;
						Buffer.Height = BackBuffer.Height;
						Buffer.Pitch = BackBuffer.Pitch;

						GameUpdateAndRender(&GameMemory, NewInput, &Buffer, &SoundBuffer);

						//locking of the buffer and unlocking it
						if (SoundIsValid) {
							win32FillSoundBuffer(&SoundOutput, ByteToLock, WriteBytes, &SoundBuffer);
						}
						win32_WindowDimensions D = GetWindowDimensions(Window);
						win32_DisplayBufferInWindow(&BackBuffer, DeviceContext,
							D.Width, D.Height);

						uint64 EndCycleCount = __rdtsc();
						LARGE_INTEGER EndCounter;
						QueryPerformanceCounter(&EndCounter);

						uint64 CyclesElapsed = EndCycleCount - LastCycleCount;

						int64 ElapsedCounter = EndCounter.QuadPart - LastCounter.QuadPart;
						int32 msPerFrame = (int32)((1000 * ElapsedCounter) / PerfCounterFrequency);
						int32 FPS = (int32)(PerfCounterFrequency / ElapsedCounter);
						int32 MCPF = (int32)(CyclesElapsed / (pow(1000, 2)));
#if 0
						char Buffer[256];
						sprintf(Buffer, " %dms/f, %df/s,  %dmc/f\n", msPerFrame, FPS, MCPF);
						OutputDebugStringA(Buffer);
#endif 
						LastCounter = EndCounter;
						LastCycleCount = EndCycleCount;
						game_input *temp;
						Swap(NewInput, OldInput);
					}
				}
			}
			else
			{
			}
		}
		else
		{
		}
		
	}
	return(0);
}

