#ifndef WAVCORE_H_INCLUDED
#define WAVCORE_H_INCLUDED

#pragma once
#include <string>
#include <vector>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>

#include "WavExceptions.h"
#include "HeaderExceptions.h"

//using namespace std;

class WavFile {
public:
    WavFile();

    WavFile(const std::string &filename);

    ~WavFile();

//    void ReadHeader( const char* filename, WavHeaderStruct* HeaderPtrAddress ) throw (IOWavException, BadFormatException);

    void CreateFromFile(const std::string &filename) throw (WavException, BadFormatException);

    std::string GetDescription() const;

    void ExtractDataInt16( const char* filename ) throw (UnsupportedFormatException, IOWavException);

    void SaveToFile( const char* filename) throw (BadParamsException, IOWavException);

    void ConvertStereoToMono() throw (BadParamsException);

    void ApplyReverb( double DelaySeconds, float decay ) throw (BadParamsException);

    int GetChanCount() const;

    bool IsStereo() const;

    int GetSampleRate() const;

private:

    std::vector<std::vector<short>> channelsData;

    struct WavHeaderStruct {

        char chunkId[4];

        unsigned long chunkSize;

        char format[4];

        char subchunk1Id[4];

        unsigned long subchunk1Size;

        unsigned short audioFormat;

        unsigned short numChannels;

        unsigned long sampleRate;

        unsigned long byteRate;

        unsigned short blockAlign;

        unsigned short bitsPerSample;

        char subchunk2Id[4];

        unsigned long subchunk2Size;
    };

    WavHeaderStruct WavHeader;

    std::string filenameWav;

    void NullHeader();

    void CheckHeader( size_t FileSizeBytes )
    throw (HeaderRiffException, HeaderFileSizeException, HeaderWaveException, HeaderFMTException, HeaderPCMException,
       HeaderSubchunk1Exception, HeaderBytesRateException, HeaderBlockAlignException, HeaderSubchunk2SizeException);

    void PreFillHeader();

    void FillHeader(int ChanCount, int BitsPerSample, int SampleRate, int SamplesCountPerChan );
};

#endif // WAVCORE_H_INCLUDED
