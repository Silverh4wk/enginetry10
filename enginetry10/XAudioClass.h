#pragma once
#pragma comment(lib, "Xaudio2.lib")

#include <xaudio2.h>
#include <stdio.h>

class XAudioClass
{
public:
	XAudioClass();
	XAudioClass(const XAudioClass&);
	~XAudioClass();

	bool Initialize();
	void Shutdown();

	IXAudio2* GetXAudio2();

private:
	//We require interfaces to XAudio2 and a master voice.

	IXAudio2* XAudio2;
	IXAudio2MasteringVoice* MasterVoice;
};