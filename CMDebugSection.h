#pragma once
#include "CFileStream.h"
#include "CObjectFile.h"
#include <vector>

struct MDebugHeader
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

	//these are accessed through the CObjectFile
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
};

class CMDebugSection
{
	MDebugHeader* m_data;						//pointer to the mdebug header
	std::vector<CObjectFile> m_object_files;	//cache of object files

public:
	CMDebugSection():
		m_data(nullptr),
		m_object_files()
	{}

	bool Load(CFileStream& f, MDebugHeader* data, unsigned int text_section_offset, unsigned int entry_point_virtual_address);
	void Dump(CFileStream& f);

	const CObjectFile* FindObjectFile(const char* source_file_name) const;
	bool MatchObjectFile(const CMDebugSection& other, const char* object_file_name) const;
};
