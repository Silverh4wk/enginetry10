//XAudio Class
// stores the information abt wave and the output device
// UGHHH I NEED TO READ MORE TO EXPLAIN BETTER 
//(ToDO: Silver): read more abt audio mixing and how it works to prepare a better document

#pragma once
#include "xaudioclass.h"

class XAudioSoundClass
{
private:

        struct RiffWaveHeaderType
    {
        char chunkId[4];
        unsigned long chunkSize;
        char format[4];
    };

    struct SubChunkHeaderType
    {
        char subChunkId[4];
        unsigned long subChunkSize;
    };

    struct FormtType
    {
        unsigned short audioFormat;
        unsigned short numChannels;
        unsigned long sampleRate;
        unsigned long bytesPerSecond;
        unsigned short blockAlign;
        unsigned short bitsPerSample;
    };

public:
    XAudioSoundClass();
    XAudioSoundClass(const XAudioSoundClass&);
    ~XAudioSoundClass();
    
    bool LoadTrack(IXAudio2*, char*, float);
    void ReleaseTrack();

    bool PlayTrack();
    bool StopTrack();

private:
    bool LoadStereoWaveFile(IXAudio2*, char*, float);
    void ReleaseWaveFile();

private:
        
    unsigned char* WaveData;
    XAUDIO2_BUFFER AudioBuffer;
    IXAudio2SourceVoice* SourceVoice;
};
