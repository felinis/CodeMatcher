#include "CElfFile.h"
#include "sections.h"
#include "CMDebugSection.h"
#include <stdio.h>
#include <string.h>

enum class ElfFileType: short
{
	NONE = 0,
	RELOCATABLE = 1,
	EXECUTABLE = 2,
	SHARED_OBJECT = 3
};

enum class ElfArchitecture: short
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

bool CElfFile::Load(const char* file_name)
{
	if (!CFileStream::DoesFileExist(file_name))
	{
		printf("File '%s' does not exist.\n", file_name);
		return false;
	}

	CFileStream f;
	if (!f.Open(file_name))
	{
		printf("Failed to open file '%s'.\n", file_name);
		return false;
	}

	//get the ELF header
	ElfHeader* header = (ElfHeader*)f.GetDataAt(0);

	//validate it
	if (header->magic != 'FLE\177' ||
		header->fileType != ElfFileType::EXECUTABLE ||
		header->architecture != ElfArchitecture::MIPS ||
		header->size != sizeof(ElfHeader) ||
		header->section_size != sizeof(ElfSectionHeader) ||
		header->nsections > 60)
	{
		printf("This is not a valid PlayStation 2 executable.\n");
		return false;
	}

	//get a pointer to the section headers
	ElfSectionHeader* sections = (ElfSectionHeader*)f.GetDataAt(header->section_headers_offset);

	//get the strings section (it contains the names of all the other sections)
	ElfSectionHeader* strings_section = &sections[header->string_section_index];
	char* section_names_offset = (char*)f.GetDataAt(strings_section->offset);

	//find the mdebug section
	int mdebug_section_index = -1;
	for (int i = 0; i < header->nsections; i++)
	{
		ElfSectionHeader& section = sections[i];
		
		//compare the names
		const char* name = section_names_offset + section.name_index;
		if (strcmp(name, ".mdebug") == 0)
		{
			//found it
			mdebug_section_index = i;
			break;
		}
	}

	if (mdebug_section_index == -1)
	{
		printf("Could not find the .mdebug section.\n");
		return false;
	}
	
	ElfSectionHeader* mdebug_header = (ElfSectionHeader*)f.GetDataAt(
		header->section_headers_offset + mdebug_section_index * sizeof(ElfSectionHeader)
	);
	CMDebugSection* mdebug_section = (CMDebugSection*)f.GetDataAt(mdebug_header->offset);
	if (!mdebug_section->Load(f))
	{
		printf("Failed to load the .mdebug section.\n");
		return false;
	}
	
	//dump all the files
	mdebug_section->Dump(f);

	return true;
}
