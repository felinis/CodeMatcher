#pragma once

class CFileStream
{
public:
	CFileStream():
		mCurrentPointer(nullptr),
		mOriginalPointer(nullptr),
		mSize(0),
		mFileStreamHandle(nullptr)
	{}
	~CFileStream()
	{
		Close();
	}

	static bool DoesFileExist(const char* FileStreamName);

	int Open(const char* FileStreamName);
	void Close();

	void Read(void* out, unsigned int numBytes);
	void GetPointer(void** out, unsigned int numBytes);
	void* GetDataAt(unsigned int offset) const;
	void Advance(unsigned int numBytes);
	float GetAdvancement();

private:
	char *mCurrentPointer;
	char *mOriginalPointer;
	unsigned long mSize;
	void *mFileStreamHandle;
};

