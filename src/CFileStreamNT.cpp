//================================
//	CodeMatcher
//	FileStreamNT.cpp
//
//	Purpose: allows to read a file using a memory mapping,
//	which is faster than reading the file using multiple fread calls.
//
//	(C)2022 fenik0
//================================

#include "CFileStream.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

namespace fs = boost::filesystem;
namespace io = boost::iostreams;

bool CFileStream::DoesFileExist(const char* file_name)
{
    return fs::exists(file_name);
}

int CFileStream::Open(const char* file_name)
{
    mFile.open(file_name);

    if (!mFile.is_open())
        return 0;

    mSize = fs::file_size(file_name);
    mOriginalPointer = (char*)mFile.data();
    mCurrentPointer = mOriginalPointer;

    return 1;
}

void CFileStream::Close()
{
    mFile.close();
}

void CFileStream::Read(void* out, unsigned int numBytes)
{
	memcpy(out, mCurrentPointer, numBytes);
	Advance(numBytes);
}

void CFileStream::GetPointer(void** out, unsigned int numBytes)
{
	*out = mCurrentPointer;
	Advance(numBytes);
}

void* CFileStream::GetDataAt(unsigned int offset) const
{
	return (void*)(mOriginalPointer + offset);
}

void CFileStream::Advance(unsigned int numBytes)
{
	mCurrentPointer += numBytes;
}

float CFileStream::GetAdvancement()
{
	return (float)((unsigned long long)mCurrentPointer - (unsigned long long)mOriginalPointer) /
		(float)mSize;
}
