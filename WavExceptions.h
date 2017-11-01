#ifndef WAV_EXCEPTIONS_H_INCLUDED
#define WAV_EXCEPTIONS_H_INCLUDED

#pragma once
#include <exception>

class WavException: public std::runtime_error {
public:
    explicit WavException(const std::string &__args): runtime_error(__args) {}
};

class IOWavException: public WavException {
public:
    explicit IOWavException(const std::string& filename): WavException("Error with input/output " + filename + " file."){}
};


class BadFormatException: public std::runtime_error {
public:
    explicit BadFormatException(const std::string& filename) : runtime_error("Error with format of " + filename + " file."){}
};


class UnsupportedFormatException: public std::runtime_error {
public:
    explicit UnsupportedFormatException(const std::string& filename) : runtime_error("Unsupported format of" + filename + " file."){}
};


class BadParamsException: public std::runtime_error {
public:
    explicit BadParamsException(const std::string& filename) : runtime_error("Bad params in " + filename + " file."){}
};


class DataSizeException: public std::runtime_error {
public:
    explicit DataSizeException(const std::string& filename) : runtime_error("Error size of data in " + filename + " file."){}
};

#endif // WAV_EXCEPTIONS_H_INCLUDED
