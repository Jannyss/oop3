#ifndef HEADER_EXCEPTIONS_H_INCLUDED
#define HEADER_EXCEPTIONS_H_INCLUDED

#pragma once
#include <exception>
#include <string>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>

//using namespace std;

class HeaderRiffException: public std::runtime_error {
public:
    explicit HeaderRiffException(const std::string& filename): runtime_error("Riff error"){}
};


class HeaderFileSizeException: public std::runtime_error {
public:
    explicit HeaderFileSizeException(const std::string& filename): runtime_error("Invalid file size in " + filename){}
};


class HeaderWaveException: public std::runtime_error {
public:
    explicit HeaderWaveException(const std::string& filename): runtime_error("Wave error"){}
};


class HeaderFMTException: public std::runtime_error {
public:
    explicit HeaderFMTException(const std::string& filename): runtime_error("FMT error"){}
};


class HeaderPCMException: public std::runtime_error {
public:
    explicit HeaderPCMException(const std::string& filename): runtime_error("PCM error"){}
};


class HeaderSubchunk1Exception: public std::runtime_error {
public:
    explicit HeaderSubchunk1Exception(const std::string& filename): runtime_error("Subchunk1 error"){}
};


class HeaderBytesRateException: public std::runtime_error {
public:
    explicit HeaderBytesRateException(const std::string& filename): runtime_error("Bytes Rate error"){}
};


class HeaderBlockAlignException: public std::runtime_error {
public:
    explicit HeaderBlockAlignException(const std::string& filename): runtime_error("Block Align error"){}
};


class HeaderSubchunk2SizeException: public std::runtime_error {
public:
    explicit HeaderSubchunk2SizeException(const std::string& filename): runtime_error("Size of Subchunk2 error"){}
};

#endif // HEADER_EXCEPTIONS_H_INCLUDED

