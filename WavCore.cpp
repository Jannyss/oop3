#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>

#include "WavCore.h"

using namespace std;

WavFile::WavFile() {};

WavFile::WavFile(const string &filename) {
    CreateFromFile(filename);
}

WavFile::~WavFile() {}

void WavFile::CreateFromFile(const std::string &filename) throw (WavException, BadFormatException) {
    filenameWav = filename;
    WavFile::NullHeader(); // Fill header with zeroes.
    ifstream file(filename, ifstream::binary);
    if ( !file) {
        throw IOWavException (filename);
    }

    file.seekg(0, ios::beg);
    file.read((char *) (&WavHeader), sizeof(WavHeaderStruct));

    if (file.gcount() != sizeof(WavHeaderStruct)) {
        // can't read header, because the file is too small.
        throw BadFormatException (filenameWav);
    }

    uint16_t channelsCount = WavHeader.numChannels;
    unsigned int samplesPerChannel = (WavHeader.subchunk2Size / sizeof(uint16_t)) / channelsCount;

    // 1. Reading all PCM data from file to a single vector.
    vector<short> allChannels;
    allChannels.resize(channelsCount * samplesPerChannel);
    file.read((char *) allChannels.data(), WavHeader.subchunk2Size);

    if (file.gcount() != WavHeader.subchunk2Size) {
        throw IOWavException(filenameWav);
    }
    file.close();

    // 2. Put all channels to its own vector.
    channelsData.resize(channelsCount);
    for (auto &ch : channelsData) {
        ch.resize(samplesPerChannel);
    }

    for (int ch = 0; ch < channelsCount; ch++) {
        vector<short> &channel = channelsData[ch];
        for (size_t i = 0; i < samplesPerChannel; i++) {
            channel[i] = allChannels[channelsCount * i + ch];
        }
    }
}

//void WavFile::ReadHeader( const char* filename, WavHeaderStruct* HeaderPtrAddress ) throw (IOWavException, BadFormatException) {
//    //filenameWav = filename;
//    WavFile::NullHeader( HeaderPtrAddress ); // Fill header with zeroes.
//
//    FILE* f = fopen( filename, "rb" );
//    if ( !f ) {
//        throw IOWavException (filename);
//    }
//
//    size_t blocksRead = fread( HeaderPtrAddress, sizeof(WavHeaderStruct), 1, f);
//    if ( blocksRead != 1 ) {
//        // can't read header, because the file is too small.
//        throw BadFormatException (filename);
//    }
//
//    fseek( f, 0, SEEK_END ); // seek to the end of the file
//    size_t fileSize = ftell( f ); // current position is a file size!
//    fclose( f );
//
//    if ( WavFile::CheckHeader( HeaderPtrAddress, fileSize ) != 0 ) {
//        throw BadFormatException (filename);
//    }
////    else {
////        return 0;
////}
//}

string WavFile::GetDescription() const {
    string stringDesc = "-------------------------\n audioFormat   " + to_string(WavHeader.audioFormat) + "\n" +
                        " numChannels   " + to_string(WavHeader.numChannels) + "\n" +
                        " sampleRate    " + to_string(WavHeader.sampleRate) + "\n" +
                        " bitsPerSample " + to_string(WavHeader.bitsPerSample) + "\n" +
                        " byteRate      " + to_string(WavHeader.byteRate) + "\n" +
                        " blockAlign    " + to_string(WavHeader.blockAlign) + "\n" +
                        " chunkSize     " + to_string(WavHeader.chunkSize) + "\n" +
                        " subchunk1Size " + to_string(WavHeader.subchunk1Size) + "\n" +
                        " subchunk2Size " + to_string(WavHeader.subchunk2Size) + "\n" +
                        "-------------------------\n";
    return stringDesc;
}


void WavFile::ExtractDataInt16( const char* filename ) throw (UnsupportedFormatException, IOWavException){
    if ( WavHeader.bitsPerSample != 16 ) {
        // Only 16-bit samples is supported.
        throw UnsupportedFormatException(filename);
    }

    ifstream file(filename, ifstream::binary);
    if ( !file) {
        throw IOWavException (filename);
    }

    file.seekg(44, ios::beg); //44 - size of header file
                              // Seek to the begining of PCM data.

    unsigned short chanCount = WavHeader.numChannels;
    unsigned int samplesPerChan = ( WavHeader.subchunk2Size / sizeof(short) ) / chanCount;

    // 1. Reading all PCM data from file to a single vector.
    vector<short> allChannels;
    allChannels.resize( chanCount * samplesPerChan );
    file.read((char *) allChannels.data(), WavHeader.subchunk2Size);
    if ( file.gcount() != WavHeader.subchunk2Size ) {
        throw IOWavException(filename);
    }
    file.close();

    // 2. Put all channels to its own vector.
    channelsData.resize( chanCount );
    for ( auto ch: channelsData) {
        ch.resize( samplesPerChan );
    }

    for ( int ch = 0; ch < chanCount; ch++ ) {
        vector<short>& chdata = channelsData[ ch ];
        for ( size_t i = 0; i < samplesPerChan; i++ ) {
            chdata[ i ] = allChannels[ chanCount * i + ch ];
        }
    }
}

void WavFile::SaveToFile( const char* filename ) throw (BadParamsException, IOWavException) {

    unsigned int chanCount = channelsData.size();

    if ( chanCount < 1 ) {
        throw BadParamsException(filename);
    }

    unsigned int samplesCountPerChan = channelsData[0].size();

    // Verify that all channels have the same number of samples.
    for ( size_t ch = 0; ch < chanCount; ch++ ) {
        if ( channelsData[ ch ].size() != (size_t) samplesCountPerChan ) {
            throw BadParamsException(filename);
        }
    }

    int bitsPerSample = 16;
    WavFile::FillHeader( chanCount, bitsPerSample, WavHeader.sampleRate, samplesCountPerChan );

    vector<short> allChannels;
    allChannels.resize( chanCount * samplesCountPerChan );

    for ( int ch = 0; ch < chanCount; ch++ ) {
        const vector<short> &channelData = channelsData[ ch ];
        for ( size_t i = 0; i < samplesCountPerChan; i++ ) {
            allChannels[ chanCount * i + ch ] = channelData[ i ];
        }
    }

    ofstream outfile(filename, ofstream::binary | ofstream::out);
    outfile.write((char *) (&WavHeader), sizeof(WavHeaderStruct));
    outfile.write((char *) allChannels.data(), allChannels.size() * sizeof(short));

    if ( !outfile ) {
        throw IOWavException(filename);
    }
    outfile.close();
}

void WavFile::ConvertStereoToMono() throw (BadParamsException) {
    int chanCount = (int)channelsData.size();

    if ( chanCount != 2 ) {
        throw BadParamsException(filenameWav);
    }

    int samplesCountPerChan = (int)channelsData[0].size();

    // Verify that all channels have the same number of samples.
    for ( size_t ch = 0; ch < chanCount; ch++ ) {
        if ( channelsData[ ch ].size() != (size_t) samplesCountPerChan ) {
            throw BadParamsException(filenameWav);
        }
    }

    vector<short> mono = channelsData[0];
    mono.resize( samplesCountPerChan );

    // Mono channel is an arithmetic mean of all (two) channels.
    for ( size_t i = 0; i < samplesCountPerChan; i++ ) {
        mono[ i ] = ( channelsData[0][i] + channelsData[1][i] ) / 2;
    }
//     channelsData.pop_back();
//     channelsData[0] = mono;
}

void WavFile::ApplyReverb( double DelaySeconds, float decay ) throw (BadParamsException) {
    unsigned int chanCount = channelsData.size();

    if ( chanCount < 1 ) {
        throw BadParamsException(filenameWav);
    }

    unsigned int samplesCountPerChan = channelsData[0].size();

    // Verify that all channels have the same number of samples.
    for ( size_t ch = 0; ch < chanCount; ch++ ) {
        if ( channelsData[ ch ].size() != (size_t) samplesCountPerChan ) {
            throw BadParamsException(filenameWav);
        }
    }

    int delaySamples = (int)(DelaySeconds * WavHeader.sampleRate);


    for ( size_t ch = 0; ch < chanCount; ch++ ) {
        vector<float> tmp;
        tmp.resize(channelsData[ch].size());

        // Convert signal from short to float
        for ( size_t i = 0; i < samplesCountPerChan; i++ ) {
            tmp[ i ] = channelsData[ ch ][ i ];
        }

        // Add a reverb
        for ( size_t i = 0; i < samplesCountPerChan - delaySamples; i++ ) {
            tmp[ i + delaySamples ] += decay * tmp[ i ];
        }

        // Find maximum signal's magnitude
        float maxMagnitude = 0.0f;
        for ( size_t i = 0; i < samplesCountPerChan - delaySamples; i++ ) {
            if ( abs(tmp[ i ]) > maxMagnitude ) {
                maxMagnitude = abs(tmp[ i ]);
            }
        }

        // Signed short can keep values from -32768 to +32767,
        // After reverb, usually there are values large 32000.
        // So we must scale all values back to [ -32768 ... 32768 ]
        float normCoef = 30000.0f / maxMagnitude;
        //printf( "max Magnitude = %.1f, coef = %.3f\n", maxMagnitude, normCoef );

        // Scale back and transform floats to shorts.
        for ( size_t i = 0; i < samplesCountPerChan; i++ ) {
            channelsData[ ch ][ i ] = (short)(normCoef * tmp[ i ]);
        }
    }
}

int WavFile::GetChanCount() const {
    return static_cast<unsigned int>(channelsData.size());
}

bool WavFile::IsStereo() const {
    if (channelsData.size() > 1) return true;
    else return false;
}

int WavFile::GetSampleRate() const {
    return WavHeader.sampleRate;
}

void WavFile::NullHeader() {
    memset(&WavHeader, 0, sizeof(WavHeaderStruct));
}

void WavFile::CheckHeader( size_t FileSizeBytes )
throw (HeaderRiffException, HeaderFileSizeException, HeaderWaveException, HeaderFMTException, HeaderPCMException,
       HeaderSubchunk1Exception, HeaderBytesRateException, HeaderBlockAlignException, HeaderSubchunk2SizeException) {
    // Go to wav_header.h for details

    if ( WavHeader.chunkId[0] != 'R' ||
         WavHeader.chunkId[1] != 'I' ||
         WavHeader.chunkId[2] != 'F' ||
         WavHeader.chunkId[3] != 'F' )
    {
        throw HeaderRiffException(filenameWav);
    }

    if ( WavHeader.chunkSize != FileSizeBytes - 8 ) {
        throw HeaderFileSizeException(filenameWav);
    }

    if ( WavHeader.format[0] != 'W' ||
         WavHeader.format[1] != 'A' ||
         WavHeader.format[2] != 'V' ||
         WavHeader.format[3] != 'E' )
    {
        throw HeaderWaveException(filenameWav);
    }

    if ( WavHeader.subchunk1Id[0] != 'f' ||
         WavHeader.subchunk1Id[1] != 'm' ||
         WavHeader.subchunk1Id[2] != 't' ||
         WavHeader.subchunk1Id[3] != ' ' )
    {
        throw HeaderFMTException(filenameWav);
    }

    if ( WavHeader.audioFormat != 1 ) {
        throw HeaderPCMException(filenameWav);
    }

    if ( WavHeader.subchunk1Size != 16 ) {
        throw HeaderSubchunk1Exception(filenameWav);
    }

    if ( WavHeader.byteRate != WavHeader.sampleRate * WavHeader.numChannels * WavHeader.bitsPerSample/8 ) {
        throw HeaderBytesRateException(filenameWav);
    }

    if ( WavHeader.blockAlign != WavHeader.numChannels * WavHeader.bitsPerSample/8 ) {
        throw HeaderBlockAlignException(filenameWav);
    }

    if ( WavHeader.subchunk2Id[0] != 'd' ||
         WavHeader.subchunk2Id[1] != 'a' ||
         WavHeader.subchunk2Id[2] != 't' ||
         WavHeader.subchunk2Id[3] != 'a' )
    {
        throw HeaderFMTException(filenameWav);
    }

    if ( WavHeader.subchunk2Size != FileSizeBytes - 44 )
    {
        throw HeaderSubchunk2SizeException(filenameWav);
    }
}

void WavFile::PreFillHeader() {
        // Go to wav_header.h for details

    WavHeader.chunkId[0] = 'R';
    WavHeader.chunkId[1] = 'I';
    WavHeader.chunkId[2] = 'F';
    WavHeader.chunkId[3] = 'F';

    WavHeader.format[0] = 'W';
    WavHeader.format[1] = 'A';
    WavHeader.format[2] = 'V';
    WavHeader.format[3] = 'E';

    WavHeader.subchunk1Id[0] = 'f';
    WavHeader.subchunk1Id[1] = 'm';
    WavHeader.subchunk1Id[2] = 't';
    WavHeader.subchunk1Id[3] = ' ';

    WavHeader.subchunk2Id[0] = 'd';
    WavHeader.subchunk2Id[1] = 'a';
    WavHeader.subchunk2Id[2] = 't';
    WavHeader.subchunk2Id[3] = 'a';

    WavHeader.audioFormat   = 1;
    WavHeader.subchunk1Size = 16;
    WavHeader.bitsPerSample = 16;

}

void WavFile::FillHeader( int ChanCount, int BitsPerSample, int SampleRate, int SamplesCountPerChan ) {
    if ( BitsPerSample != 16 ) {
        throw UnsupportedFormatException (filenameWav);
    }

    if ( ChanCount < 1 ) {
        throw BadParamsException (filenameWav);
    }

    WavFile::PreFillHeader();

    int fileSizeBytes = 44 + ChanCount * (BitsPerSample/8) * SamplesCountPerChan;

    WavHeader.sampleRate    = SampleRate;
    WavHeader.numChannels   = ChanCount;
    WavHeader.bitsPerSample = 16;

    WavHeader.chunkSize     = fileSizeBytes - 8;
    WavHeader.subchunk2Size = fileSizeBytes - 44;

    WavHeader.byteRate      = WavHeader.sampleRate * WavHeader.numChannels * WavHeader.bitsPerSample/8;
    WavHeader.blockAlign    = WavHeader.numChannels * WavHeader.bitsPerSample/8;

}
