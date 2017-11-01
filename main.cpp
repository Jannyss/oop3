#include <iostream>
#include <exception>
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

int main()
{
    try {

        WavFile wav1;
        wav1.CreateFromFile("0.wav");
        cout << wav1.GetDescription() << endl;
        wav1.ConvertStereoToMono();
        wav1.ApplyReverb(0.500, 0.6f);
        wav1.SaveToFile("out.wav");
        int chanCount = wav1.GetChanCount();
        cout << chanCount << endl;
        bool isStereo = wav1.IsStereo();
        cout << isStereo << endl;
        int sampleRate = wav1.GetSampleRate();
        cout << sampleRate;

    }
    catch (runtime_error &e) {
        cerr << e.what() << endl;
    }
    return 0;
}
