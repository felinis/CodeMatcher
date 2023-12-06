#pragma once
#include "sections.h"
#include "CFileStream.h"

enum class ElfFileType : short
{
	NONE = 0,
	RELOCATABLE = 1,
	EXECUTABLE = 2,
	SHARED_OBJECT = 3
};

enum class ElfArchitecture : short
{
	MIPS = 8
};

struct ElfHeader
{
	int magic;

	char dummyStuff[12];

	ElfFileType	fileType;
	ElfArchitecture	architecture;

	int	fileVersion;
	int	entryPointVirtualAddress;

	int	program_headers_offset;
	int	section_headers_offset;

	int cpu_flags;
	short size;

	short program_size;
	short nprograms;

	short section_size;
	short nsections;
	short string_section_index;
};

class CElfFile
{
protected:
	CFileStream m_f;
	ElfHeader* m_header;
	unsigned int m_text_section_offset;
	unsigned int m_entry_point_virtual_offset;

public:
	CElfFile():
		m_f(),
		m_header(nullptr),
		m_text_section_offset(0),
		m_entry_point_virtual_offset(0)
	{}

	ElfSectionHeader* GetSectionHeaders();
	const char* GetSectionNamesOffset();

	bool Load(const char* file_name);
};
