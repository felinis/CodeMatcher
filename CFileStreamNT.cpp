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
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool CFileStream::DoesFileExist(const char* file_name)
{
	return GetFileAttributes(file_name) != 0;
}

int CFileStream::Open(const char* file_name)
{
	mFileStreamHandle = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (mFileStreamHandle == INVALID_HANDLE_VALUE)
		return 0;

	mSize = GetFileSize(mFileStreamHandle, 0);

	HANDLE hMap = CreateFileMapping(mFileStreamHandle, 0, PAGE_READONLY, 0, 0, 0);
	if (!hMap)
		return 0;

	mOriginalPointer = (char*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if (!mOriginalPointer)
		return 0;

	mCurrentPointer = mOriginalPointer;

	CloseHandle(hMap);
	return 1;
}

void CFileStream::Close()
{
	UnmapViewOfFile(mOriginalPointer);
	CloseHandle(mFileStreamHandle);
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
