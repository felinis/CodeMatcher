#pragma once
#include "CFileStream.h"
#include "CObjectFile.h"

class CMDebugSection
{
	short magic;
	short version;

	unsigned int nlines;
	unsigned int lines_size;
	unsigned int lines_offset;

	unsigned int unused1;
	unsigned int unused2;

	unsigned int nprocedures;
	unsigned int procedures_offset;

	//these are accessed through the file descriptors
	unsigned int nlocalsymbols;
	unsigned int localsymbols_offset;

	unsigned int unused3;
	unsigned int unused4;

	unsigned int nauxsymbols;
	unsigned int auxsymbols_offset;

	unsigned int nlocalstrings;
	unsigned int localstrings_offset;

	unsigned int nexternalstrings;			//contains global variable names
	unsigned int externalstrings_offset;

	unsigned int nobjectfiles;
	unsigned int objectfiles_offset;

	unsigned int nrelfiles;
	unsigned int relfiles_offset;

	unsigned int nexternalsymbols;
	unsigned int externalsymbols_offset;	//shared symbols in the whole program
public:
	bool Load(CFileStream& f);
	void Dump(CFileStream& f);

	const CObjectFile& GetObjectFile(CFileStream& f, int index) const;
	const CObjectFile* GetObjectFile(CFileStream& f, const char* source_file_name) const;

	bool Compare(CFileStream& f, const CMDebugSection& other) const;
};

