#include "xaudiosoundclass.h"


XAudioSoundClass::XAudioSoundClass()
{
    WaveData = 0;
    SourceVoice = 0;
}


XAudioSoundClass::XAudioSoundClass(const XAudioSoundClass& other)
{}


XAudioSoundClass::~XAudioSoundClass()
{}


bool XAudioSoundClass::LoadTrack(IXAudio2 * XAudio2, char* filename, float volume)
{
    bool result;


    // Load the wave file for the sound.
    result = LoadStereoWaveFile(XAudio2, filename, volume);
    if (!result)
    {
        return false;
    }

    return true;
}


void XAudioSoundClass::ReleaseTrack()
{
    // Destory the voice.
    if (SourceVoice)
    {
        SourceVoice->DestroyVoice();
        SourceVoice = 0;
    }

    // Release the wave data only after voice destroyed.
    ReleaseWaveFile();

    return;
}


bool XAudioSoundClass::PlayTrack()
{
    HRESULT result;


    // Play the track.
    result = SourceVoice->Start(0, XAUDIO2_COMMIT_NOW);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}


bool XAudioSoundClass::StopTrack()
{
    HRESULT result;


    // Play the track.
    result = SourceVoice->Stop(0);
    if (FAILED(result))
    {
        return false;
    }

    // Flush the buffers to remove them and reset the audio position to the beginning.
    result = SourceVoice->FlushSourceBuffers();
    if (FAILED(result))
    {
        return false;
    }

    // Resubmit the buffer to the source voice after the reset so it is prepared to play again.
    result = SourceVoice->SubmitSourceBuffer(&AudioBuffer);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool XAudioSoundClass::LoadStereoWaveFile(IXAudio2 * xAudio2, char* filename, float volume)
{
    FILE* filePtr;
    RiffWaveHeaderType riffWaveFileHeader;
    SubChunkHeaderType subChunkHeader;
    FormtType fmtData;
    WAVEFORMATEX waveFormat;
    int error;
    unsigned long long count;
    long seekSize;
    unsigned long dataSize;
    bool foundFormat, foundData;
    HRESULT result;

        // Open the wave file in binary.
        error = fopen_s(&filePtr, filename, "rb");
    if (error != 0)
    {
        return false;
    }

    // Read in the riff wave file header.
    count = fread(&riffWaveFileHeader, sizeof(riffWaveFileHeader), 1, filePtr);
    if (count != 1)
    {
        return false;
    }

    // Check that the chunk ID is the RIFF format.
    if ((riffWaveFileHeader.chunkId[0] != 'R') || (riffWaveFileHeader.chunkId[1] != 'I') || (riffWaveFileHeader.chunkId[2] != 'F') || (riffWaveFileHeader.chunkId[3] != 'F'))
    {
        return false;
    }

    // Check that the file format is the WAVE format.
    if ((riffWaveFileHeader.format[0] != 'W') || (riffWaveFileHeader.format[1] != 'A') || (riffWaveFileHeader.format[2] != 'V') || (riffWaveFileHeader.format[3] != 'E'))
    {
        return false;
    }

    // Read in the sub chunk headers until you find the format chunk.
    foundFormat = false;
    while (foundFormat == false)
    {
        // Read in the sub chunk header.
        count = fread(&subChunkHeader, sizeof(subChunkHeader), 1, filePtr);
        if (count != 1)
        {
            return false;
        }

        // Determine if it is the format header.  If not then move to the end of the chunk and read in the next one.
        if ((subChunkHeader.subChunkId[0] == 'f') && (subChunkHeader.subChunkId[1] == 'm') && (subChunkHeader.subChunkId[2] == 't') && (subChunkHeader.subChunkId[3] == ' '))
        {
            foundFormat = true;
        }
        else
        {
            fseek(filePtr, subChunkHeader.subChunkSize, SEEK_CUR);
        }
    }

    // Read in the format data.
    count = fread(&fmtData, sizeof(fmtData), 1, filePtr);
    if (count != 1)
    {
        return false;
    }

    // Check that the audio format is WAVE_FORMAT_PCM.
    if (fmtData.audioFormat != WAVE_FORMAT_PCM)
    {
        return false;
    }

    // Check that the wave file was recorded in stereo or mono format depending on what we are expecting the file to be.
    if (fmtData.numChannels != 2)
    {
        return false;
    }

    // Check that the wave file was recorded at a sample rate of 48.0 KHz.
    if (fmtData.sampleRate != 48000)
    {
        return false;
    }

    // Ensure that the wave file was recorded in 16 bit format.
    if (fmtData.bitsPerSample != 16)
    {
        return false;
    }

    // Seek up to the next sub chunk.
    seekSize = subChunkHeader.subChunkSize - 16;
    fseek(filePtr, seekSize, SEEK_CUR);

    // Read in the sub chunk headers until you find the data chunk.
    foundData = false;
    while (foundData == false)
    {
        // Read in the sub chunk header.
        count = fread(&subChunkHeader, sizeof(subChunkHeader), 1, filePtr);
        if (count != 1)
        {
            return false;
        }

        // Determine if it is the data header.  If not then move to the end of the chunk and read in the next one.
        if ((subChunkHeader.subChunkId[0] == 'd') && (subChunkHeader.subChunkId[1] == 'a') && (subChunkHeader.subChunkId[2] == 't') && (subChunkHeader.subChunkId[3] == 'a'))
        {
            foundData = true;
        }
        else
        {
            fseek(filePtr, subChunkHeader.subChunkSize, SEEK_CUR);
        }
    }

    // Store the size of the data chunk.
    dataSize = subChunkHeader.subChunkSize;

    // Create a buffer to hold the wave file data.
    WaveData = new unsigned char[dataSize];

    // Read in the wave file data into the newly created buffer.
    count = fread(WaveData, 1, dataSize, filePtr);
    if (count != dataSize)
    {
        return false;
    }

    // Close the file once done reading.
    error = fclose(filePtr);
    if (error != 0)
    {
        return false;
    }

        // Set the wave format for the buffer.
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = fmtData.sampleRate;
    waveFormat.wBitsPerSample = fmtData.bitsPerSample;
    waveFormat.nChannels = fmtData.numChannels;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;

        // Fill in the audio buffer struct.
    AudioBuffer.AudioBytes = dataSize;          // Size of the audio buffer in bytes.
    AudioBuffer.pAudioData = WaveData;        // Buffer containing audio data.
    AudioBuffer.Flags = XAUDIO2_END_OF_STREAM ;  // Tell the source voice not to expect any data after this buffer.
    AudioBuffer.LoopCount = XAUDIO2_LOOP_INFINITE ;                  // Not looping.  Change to XAUDIO2_LOOP_INFINITE for looping. 
    AudioBuffer.PlayBegin = 0;
    AudioBuffer.PlayLength = 0;
    AudioBuffer.LoopBegin = 0;
    AudioBuffer.LoopLength = 0;
    AudioBuffer.pContext = NULL;

        // Create the source voice for this buffer.	
        result = xAudio2->CreateSourceVoice(&SourceVoice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL);
    if (FAILED(result))
    {
        return false;
    }

        // Submit the audio buffer to the source voice.
        result = SourceVoice->SubmitSourceBuffer(&AudioBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Set volume of the buffer.
    SourceVoice->SetVolume(volume);

    return true;
}


void XAudioSoundClass::ReleaseWaveFile()
{
    // Release the secondary sound buffer.
    if (WaveData)
    {
        delete[] WaveData;
        WaveData = 0;
    }

    return;
}
